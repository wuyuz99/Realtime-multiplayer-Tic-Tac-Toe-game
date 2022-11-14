package edu.scu.coen275.TicTacToe.exception;

public class RoomFullException extends Exception{
    private String message;
    public RoomFullException(String message){
        this.message = message;
    }

    @Override
    public String getMessage() {
        return message;
    }
}
