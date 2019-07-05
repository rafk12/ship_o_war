package com.battleships.server

import java.net.Socket
import java.util.logging.Logger

class Client(private val options: ServerStore, private val socket: Socket) {

    val log = Logger.getLogger("Client ${socket.remoteSocketAddress}").apply {
        useParentHandlers = false
        addHandler(BattleshipHandler())
    }

    var gridSent = false
    var skinCode = 0


    val input = socket.getInputStream() ?: throw IllegalStateException("${log.name} does not have input stream")
    val output = socket.getOutputStream() ?: throw IllegalStateException("${log.name} does not have output stream")

    init {
        log.info("Joining game")
    }

    private fun write(vararg bytes: Byte) {
        output.write(bytes)
    }

    private val readingBuffer = ByteArray(16)

    fun tick() {
        if (!gridSent) {
            gridSent = true
            write(0x01, options.size.width.toByte(), options.size.height.toByte())
        }
        if (input.available() == 0) {
            return
        }
        val byte = input.read()
    }

}