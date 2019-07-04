package com.battleships.server

import com.xenomachina.argparser.ArgParser
import com.xenomachina.argparser.default
import java.net.InetAddress
import java.net.ServerSocket
import java.net.Socket

class ServerArgs(parser: ArgParser) {

    val port by parser.storing(
        "-p", "--port",
        help = "Listening port"
    ) { toInt() }.default(49376)

    val host by parser.storing(
        "-a", "--host",
        help = "Listening address"
    ).default("0.0.0.0")

}

class Server(args: ServerArgs) {

    val socketServer = ServerSocket(args.port, 0, InetAddress.getByName(args.host))

    lateinit var p1: Socket
    lateinit var p2: Socket

    operator fun invoke() {
        println("Waiting players")
        p1 = socketServer.accept()
        println("Player 1 joined " + p1.remoteSocketAddress.toString())
        p2 = p1 //p2 = socketServer.accept()
        p1.getOutputStream().run {
            write(byteArrayOf(1, 127, 60))
            flush()
        }
        while(p1.isConnected || p2.isConnected) {
            Thread.sleep(100)
        }
    }

}

fun main(args: Array<String>) {
    Server(ArgParser(args).parseInto(::ServerArgs))()
}