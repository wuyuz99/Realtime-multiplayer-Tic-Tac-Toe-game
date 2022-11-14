package edu.scu.coen275.TicTacToe.storage;

import edu.scu.coen275.TicTacToe.model.Game;

import java.util.HashMap;
import java.util.Map;

public class GameStorage {
    private static Map<String, Game> games;
    private static volatile GameStorage instance;

    private GameStorage() {
        games = new HashMap<>();
    }

    public static GameStorage getInstance() {
        if (instance == null) {
            synchronized (GameStorage.class) {
                if (instance == null) {
                    instance = new GameStorage();
                }
            }
        }
        return instance;
    }

    public Map <String, Game> getGames() {
        return games;
    }

    public void setGame(Game game) {
        games.put(game.getGameId(), game);
    }

    public static boolean gameExists(String id) {
        return games.containsKey(id);
    }
}
