package com.battleships.server

import com.xenomachina.argparser.ArgParser
import com.xenomachina.argparser.SystemExitException
import com.xenomachina.argparser.mainBody

fun main(args: Array<String>) = mainBody {
    try {
        startServer(ArgParser(args).parseInto(::ServerArgs))
    } catch (e: IllegalArgumentException) {
        throw SystemExitException(e.message ?: "", 1)
    }
}