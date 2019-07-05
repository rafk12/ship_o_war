package com.battleships.server

typealias Coord = Pair<Int, Int>

class Boat(x: Int, y: Int, val width: Int, val height: Int) {

    var y = y
        private set
    var x = x
        private set

    var life = width * height
        private set

    fun removeLife() {
        life--
    }

    fun generatePairs(): List<Coord> = List(width * height) {
        it % width + x to it / width + y
    }

    fun overlap(data: Map<Pair<Int, Int>, Boat>): Boolean {
        for (i in 0 until width * height) {
            if (data.containsKey(i % width + x to i / width + y)) {
                return true
            }
        }
        return false
    }

    fun ensureInsideGrid(grid: GridSize) {
        if (x + width > grid.width) {
            x = grid.width - width
        }
        if (y + height > grid.height) {
            y = grid.height
        }
    }

}