package edu.scu.coen275.TicTacToe.service;

import edu.scu.coen275.TicTacToe.exception.GameNotExistException;
import edu.scu.coen275.TicTacToe.model.Game;
import edu.scu.coen275.TicTacToe.model.GameStatus;
import edu.scu.coen275.TicTacToe.model.Player;
import edu.scu.coen275.TicTacToe.storage.GameStorage;
import lombok.AllArgsConstructor;
import org.springframework.stereotype.Service;

import java.util.UUID;

@Service
@AllArgsConstructor
public class GameService {
    public Game createGame(Player player) {
        Game game = new Game();
        game.setBoard(new int[3][3]);
        game.setGameId(UUID.randomUUID().toString());
        game.setPlayer1(player);
        game.setStatus(GameStatus.NEW);
        GameStorage.getInstance().setGame(game);
        return game;
    }

    public Game connectToGame(Player player2, String gameId) throws GameNotExistException {
        if (!GameStorage.gameExists(gameId)) {
            throw new GameNotExistException("Game with id not exists");
        }
        Game game = GameStorage.getInstance().getGames().get(gameId);
        return game;
    }
}
