package edu.scu.coen275.TicTacToe.dto;

import edu.scu.coen275.TicTacToe.model.Player;
import lombok.Data;

@Data
public class ConnectRequest {
    private Player player;
    private String gameId;
}
