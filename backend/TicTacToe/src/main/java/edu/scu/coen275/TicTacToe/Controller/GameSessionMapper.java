package edu.scu.coen275.TicTacToe.Controller;

import org.springframework.web.socket.WebSocketSession;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class GameSessionMapper {
    private static GameSessionMapper instance = null;
    public Map<String, List<WebSocketSession>> mapping;
    private GameSessionMapper() {
        mapping = new HashMap<>();
    }

    public static GameSessionMapper getInstance()
    {
        if (instance == null)
            instance = new GameSessionMapper();

        return instance;
    }
}
