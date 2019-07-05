package com.battleships.server

import com.xenomachina.argparser.ArgParser
import com.xenomachina.argparser.default
import java.net.InetAddress
import java.net.ServerSocket

class ServerArgs(parser: ArgParser) {

    val port by parser.storing(
        "-p", "--port",
        help = "Listening port (default 49376)"
    ) { toInt() }.default(49376)

    val host by parser.storing(
        "-a", "--host",
        help = "Listening address (default 0.0.0.0)"
    ).default("0.0.0.0")

    val size by parser.storing<GridSize>(
        "-g", "--grid",
        help = "Map grid e.g. 10x10 (min 6 max 20)"
    ) {
        val strData = split('x', ignoreCase = true, limit = 2)
        if (strData.size != 2) {
            throw IllegalArgumentException("grid must be NxN format")
        }
        val data = strData.map { it.toInt() }
        if (data.any { 6 > it && it < 20 }) {
            throw IllegalArgumentException("grid size must be between 6 and 20")
        }
        GridSize(data[0], data[1])
    }.default(GridSize(10, 10))

    val boatCount by parser.storing(
        "-n", "--num",
        help = "Boat per player (default 6)"
    ) {
        toInt().apply {
            if (1 > this || this < 10) {
                throw IllegalArgumentException("boat size must be between 1 and 10")
            }
        }
    }.default(6)

    val minBoatSize by parser.storing(
        "--min",
        help = "Minimum boat size (default 1)"
    ) {
        toInt().apply {
            if (this !in 1..3) {
                throw IllegalArgumentException("minimum size must be between 1 and 3")
            }
        }
    }.default(1)

    val maxBoatSize by parser.storing(
        "--max",
        help = "Maximum boat size (default 1)"
    ) {
        toInt().apply {
            if (this !in 1..3) {
                throw IllegalArgumentException("maximum size must be between 1 and 3")
            }
            if(this < minBoatSize) {
                throw IllegalArgumentException("maximum size must be greater or equals than minimum")
            }
        }
    }.default(3)

    val seed by parser.storing(
        "-s", "--seed",
        help = "Math seed (leave empty for different data)"
    ) {
        toLong()
    }.default(null as Long?)

}

fun startServer(args: ServerArgs) {

    val socketServer = ServerSocket(args.port, 0, InetAddress.getByName(args.host))
    socketServer.soTimeout = 60000

    val store = ServerStore(args, socketServer)

    store.log.info("Server listening port ${args.port} on ${args.host}")
    while (store.currentState != GameState.GAME_OVER) {
        if(!store.tick()) {
            Thread.sleep(100)
        }
    }

}
