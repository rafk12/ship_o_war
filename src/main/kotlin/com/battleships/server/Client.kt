package com.battleships.server

import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.net.Socket
import java.util.logging.Logger
import kotlin.math.ceil
import kotlin.random.Random

class Client(private val store: ServerStore, socket: Socket) {

    enum class State {
        NONE,
        SKIN,
        CLICK,
        CLICK2
    }

    enum class SlotState(val code: Int) {
        Intact(0),
        Smoke(1),
        Fire(2),
        Explode(3)
    }

    fun getSlotStateFromPercentage(code: Float): SlotState {
        return SlotState.values().first { it.code == ceil(code / 4.0).toInt() }
    }

    val log = Logger.getLogger("Client ${socket.remoteSocketAddress}").apply {
        useParentHandlers = false
        addHandler(BattleshipHandler())
    }

    var gridSent = false
    var skinCode = 0
    var requiresNewSocket = false

    val boats = mutableMapOf<Pair<Int, Int>, Boat>()

    var socket = socket
        set(value) {
            field = value
            setupStreams()
        }

    lateinit var input: InputStream
    lateinit var output: OutputStream

    init {
        log.info("Joining game")
        setupStreams()
        if (boats.isEmpty()) {
            log.info("Generating boats")
            val args = store.args
            val rand = if (args.seed != null) Random(args.seed!!) else Random.Default
            (1..args.boatCount).forEach { _ ->
                var width: Int
                var height: Int
                while (true) {
                    val value = rand.nextInt(args.minBoatSize, args.maxBoatSize + 1)
                    if (rand.nextBoolean()) {
                        width = 1
                        height = value
                    } else {
                        width = value
                        height = 1
                    }
                    val b = Boat(rand.nextInt(args.size.width), rand.nextInt(args.size.height), width, height)
                    b.ensureInsideGrid(args.size)
                    if (b.overlap(boats)) {
                        continue
                    }
                    boats.putAll(b.generatePairs().map { it to b })
                    break
                }
            }
        }
    }

    private fun setupStreams() {
        socket.soTimeout = 1
        input = socket.getInputStream() ?: throw IllegalStateException("${log.name} does not have input stream")
        output = socket.getOutputStream() ?: throw IllegalStateException("${log.name} does not have output stream")
    }

    private fun write(vararg bytes: Byte) {
        output.write(bytes)
    }

    private fun sendSlot(x: Int, y: Int, width: Int, height: Int, state: SlotState) {
        write(0x3, x.toByte(), y.toByte(), width.toByte(), height.toByte(), state.code.toByte())
    }

    private fun sendSkin(byte: Byte) {
        write(0x2, byte)
    }

    private fun sendWin() {
        write(0x4, 1)
        store.clients.forEach { if (it != this) write(0x4, 0) }
    }

    private fun sendClick(x: Int, y: Int) {
        if (store.currentClient != this) {
            return
        }
        val other = store.clients.first { it != this }
        val foundBoat = other.boats.remove(x to y)
        if (foundBoat != null) {
            foundBoat.removeLife()
            if (foundBoat.life == 0) {
                other.sendSlot(-foundBoat.x, -foundBoat.y, foundBoat.width, foundBoat.height, SlotState.Explode)
            } else {
                other.sendSlot(
                    -x,
                    -y,
                    1,
                    1,
                    getSlotStateFromPercentage(foundBoat.life.toFloat() / (foundBoat.width * foundBoat.height))
                )
            }
        }
        if (other.boats.isEmpty()) {
            sendWin()
        }
        store.currentClient = other
    }

    fun sendTurn(me: Boolean) {
        write(0x6, (if (me) 1 else 0).toByte())
    }

    private val readingBuffer = ByteArray(16)

    private var state = State.NONE

    fun tick(): Boolean {
        if (socket.isClosed) {
            gridSent = false
            requiresNewSocket = true
            log.info("Disconnected, waiting new player")
            return false
        }
        if (!gridSent) {
            gridSent = true
            write(1, store.args.size.width.toByte(), store.args.size.height.toByte())
            boats.values.toSet().forEach { boat ->
                sendSlot(boat.x, boat.y, boat.width, boat.height, SlotState.Intact)
            }
        }
        val available = input.available()
        var byte = 0
        try {
            byte = input.read()
        } catch (ios: IOException) {
        }
        if (byte == -1) {
            socket.close()
        }
        if (available == 0) {
            return false
        }
        when (state) {
            State.NONE -> state = when (byte) {
                0 -> {
                    State.SKIN
                }
                1 -> {
                    State.CLICK
                }
                else -> {
                    State.NONE
                }
            }
            State.SKIN -> {
                state = State.NONE
                store.clients.find { it != this }?.sendSkin(byte.toByte())
            }
            State.CLICK -> {
                state = State.CLICK2
                temp = byte
            }
            State.CLICK2 -> {
                state = State.NONE
                store.clients.find { it != this }?.sendClick(temp, byte)
            }
        }
        return true
    }

    var temp: Int = 0

}