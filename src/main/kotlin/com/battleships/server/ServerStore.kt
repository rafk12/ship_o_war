package com.battleships.server

import java.io.IOException
import java.net.ServerSocket
import java.net.SocketTimeoutException
import java.util.logging.Logger

enum class GameState {
    WAITING_PLAYERS,
    FIRST,
    SECOND,
    GAME_OVER
}

class GridSize(val width: Int, val height: Int)

class ServerStore(args: ServerArgs, val socketServer: ServerSocket) {

    val log = Logger.getLogger("Server").apply {
        addHandler(BattleshipHandler())
        useParentHandlers = false
    }

    var currentState = GameState.WAITING_PLAYERS

    val clients = mutableListOf<Client>()

    val size = args.size

    fun tick() {
        if (currentState == GameState.WAITING_PLAYERS) {
            if (clients.any { !it.gridSent }) {
                return
            }
            currentState = GameState.FIRST
        } else if (clients.size < 2) {
            log.info("Waiting ${2 - clients.size} players...")
            try {
                clients += Client(this, socketServer.accept())
            } catch (e: SocketTimeoutException) {
            }
        }
        for (c in clients) {
            try {
                c.tick()
            } catch (ioe: IOException) {
                c.log.info("[IOException] Disconnected")
                clients.remove(c)
                return
            }
        }
    }

}
