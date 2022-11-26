package edu.scu.coen275.TicTacToe.Controller;

import edu.scu.coen275.TicTacToe.storage.GameStorage;
import org.springframework.web.socket.WebSocketSession;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class GameSessionMapper {
    private static Map<String, List<WebSocketSession>> mapping;
    private static volatile GameSessionMapper instance = null;
    private GameSessionMapper() {
        mapping = new HashMap<>();
    }

    public static GameSessionMapper getInstance()
    {
        if (instance == null) {
            synchronized (GameStorage.class) {
                if (instance == null) {
                    instance = new GameSessionMapper();
                }
            }
        }
        return instance;
    }

    public Map<String, List<WebSocketSession>> getMapping() {
        return mapping;
    }
}
