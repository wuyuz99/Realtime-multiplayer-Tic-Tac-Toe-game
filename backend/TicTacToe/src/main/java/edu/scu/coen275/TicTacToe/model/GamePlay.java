package edu.scu.coen275.TicTacToe.model;

import lombok.Data;

@Data
public class GamePlay {
    private TicToe type;
    private Integer coordX;
    private Integer coordY;
    private String gameId;
}

