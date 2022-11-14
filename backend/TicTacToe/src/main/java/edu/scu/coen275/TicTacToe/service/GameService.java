package edu.scu.coen275.TicTacToe.service;

import edu.scu.coen275.TicTacToe.exception.InvalidGameException;
import edu.scu.coen275.TicTacToe.exception.NotFoundException;
import edu.scu.coen275.TicTacToe.exception.RoomFullException;
import edu.scu.coen275.TicTacToe.model.*;
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

    public Game connectToGame(Player player2, String gameId) throws NotFoundException, RoomFullException {
        if (!GameStorage.gameExists(gameId)) {
            throw new NotFoundException("Game with id do not exists");
        }
        Game game = GameStorage.getGame(gameId);
        if (game.getPlayer2() != null) {
            throw new RoomFullException("Room full") ;
        }
        game.setPlayer2(player2);
        game.setStatus(GameStatus.IN_PROGRESS);
        GameStorage.getInstance().setGame(game);
        return game;
    }

    public Game gamePlay(GamePlay gamePlay) throws NotFoundException, InvalidGameException {
        if (!GameStorage.gameExists(gamePlay.getGameId())) {
            throw new NotFoundException("Game with id do not exists");
        }
        Game game = GameStorage.getGame(gamePlay.getGameId());
        if (game.getStatus().equals(GameStatus.FINISHED)) {
            throw new InvalidGameException("Game already finished");
        }
        int[][] board = game.getBoard();
        board[gamePlay.getCoordX()][gamePlay.getCoordY()] = gamePlay.getType().getValue();
        checkWinner(board, gamePlay.getType());
        game.setWinner(gamePlay.getType());
        GameStorage.getInstance().setGame(game);
        return game;
    }

    private Boolean checkWinner(int[][] board, TicToe ticToe) {
        int first, second, third, value;
        value = ticToe.getValue();
        for (int row = 0; row < 3; ++row) {
            first = board[row][0];
            second = board[row][1];
            third = board[row][2];
            if (first == value && first == second && second == third) {
                return true;
            }
        }
        for (int col = 0; col < 3; ++col) {
            first = board[0][col];
            second = board[1][col];
            third = board[2][col];
            if (first == value && first == second && second == third) {
                return true;
            }
        }
        first = board[0][0];
        second = board[1][1];
        third = board[2][2];
        if (first == value && first == second && second == third) {
            return true;
        }
        first = board[0][2];
        second = board[1][1];
        third = board[2][0];
        if (first == value && first == second && second == third) {
            return true;
        }
        return false;
    }
}