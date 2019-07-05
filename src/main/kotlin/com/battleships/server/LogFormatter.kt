package com.battleships.server

import java.text.SimpleDateFormat
import java.util.*
import java.util.logging.ConsoleHandler
import java.util.logging.Formatter
import java.util.logging.LogRecord

class BattleshipHandler : ConsoleHandler() {

    companion object {
        val baseFormatter = object : Formatter() {
            val dateFormatter = SimpleDateFormat("hh:mm::ss a")
            override fun format(record: LogRecord): String {
                return "${dateFormatter.format(Date(record.millis))} ${record.loggerName}: ${record.message}\n"
            }
        }
    }

    init {
        formatter = baseFormatter
    }
}
