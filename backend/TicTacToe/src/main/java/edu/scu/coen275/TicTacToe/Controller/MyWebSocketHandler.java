package edu.scu.coen275.TicTacToe.Controller;

import edu.scu.coen275.TicTacToe.model.Game;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.HashMap;
import java.util.Map;

@Component
public class MyWebSocketHandler extends TextWebSocketHandler {
    @Override
    public void handleTransportError(WebSocketSession session, Throwable throwable) throws Exception {
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
        String uri = session.getUri().toString();
        String[] tokens = uri.split("/");
        String roomId = tokens[tokens.length - 1];
        GameSessionMapper mapper = GameSessionMapper.getInstance();
        mapper.mapping.remove(roomId);
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        String uri = session.getUri().toString();
        String[] tokens = uri.split("/");
        String roomId = tokens[tokens.length - 1];
        GameSessionMapper mapper = GameSessionMapper.getInstance();
        mapper.mapping.put(roomId, session);
    }

    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage jsonTextMessage) throws Exception {
        session.sendMessage(new TextMessage("test"));
    }
}

