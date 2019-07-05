package com.battleships.server

import java.io.IOException
import java.net.ServerSocket
import java.net.SocketTimeoutException
import java.util.logging.Logger

enum class GameState {
    WAITING_PLAYERS,
    PLAYING,
    GAME_OVER
}

class GridSize(val width: Int, val height: Int)

class ServerStore(val args: ServerArgs, val socketServer: ServerSocket) {

    val log = Logger.getLogger("Server").apply {
        addHandler(BattleshipHandler())
        useParentHandlers = false
    }

    var currentState = GameState.WAITING_PLAYERS

    val clients = mutableListOf<Client>()

    var currentClient: Client? = null
    set(value) {
        field = value
        clients.forEach {
            it.sendTurn(it == value)
        }
    }

    fun tick(): Boolean {
        var anyData = false
        if (currentState == GameState.WAITING_PLAYERS) {
            if (clients.any { !it.gridSent }) {
                return false
            }
            currentState = GameState.PLAYING
        } else if (clients.size < 2) {
            log.info("Waiting ${2 - clients.size} players...")
            try {
                val cl = Client(this, socketServer.accept())
                clients += cl
                currentClient = cl
            } catch (e: SocketTimeoutException) {
            }
        }
        if (currentState == GameState.PLAYING) {
            for (c in clients) {
                try {
                    if (c.requiresNewSocket) {
                        c.requiresNewSocket = false
                        c.socket = socketServer.accept()
                        c.log.info("New player connected")
                        c.sendTurn(currentClient == c)
                    }
                } catch (ioe: IOException) {
                    c.log.info("Waiting player reconnect")
                    return anyData
                }
                try {
                    if (!c.requiresNewSocket && c.tick()) {
                        anyData = true
                    }
                } catch (ioe: IOException) {
                    if(!c.requiresNewSocket) {
                        c.log.info("[IOException] Disconnected for unknown reason")
                        clients.remove(c)
                        return anyData
                    }
                }
            }
        }
        return anyData
    }

}
