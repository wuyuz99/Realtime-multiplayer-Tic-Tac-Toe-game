package edu.scu.coen275.TicTacToe.exception;

public class GameNotExistException extends Exception {
    private String message;
    public GameNotExistException(String message){
        this.message = message;
    }

    @Override
    public String getMessage() {
        return message;
    }
}
