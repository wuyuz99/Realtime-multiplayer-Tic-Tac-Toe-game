package edu.scu.coen275.TicTacToe.model;

import lombok.Data;

@Data
public class Game {
    private String gameId;
    private Player player1;
    private Player player2;
    private GameStatus status;
    private int [][] board;
    private GamePlay[] steps;
    private int currStep;
    private TicToe winner;
}
