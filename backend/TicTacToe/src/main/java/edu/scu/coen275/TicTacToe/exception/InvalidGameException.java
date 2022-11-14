package edu.scu.coen275.TicTacToe.exception;

public class InvalidGameException extends Throwable {
    private String message;
    public InvalidGameException(String message){
        this.message = message;
    }

    @Override
    public String getMessage() {
        return message;
    }
}
