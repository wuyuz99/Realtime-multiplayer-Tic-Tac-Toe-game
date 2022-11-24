package edu.scu.coen275.TicTacToe.Controller;

import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.ArrayList;


@Component
@Slf4j
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
        if (mapper.mapping.containsKey(roomId)) {
            mapper.mapping.get(roomId).remove(session);
            if (mapper.mapping.get(roomId).size() == 0) {
                mapper.mapping.remove(roomId);
            }
        }
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        log.info("webSocketConnection: {}", session);
        String uri = session.getUri().toString();
        String[] tokens = uri.split("/");
        String roomId = tokens[tokens.length - 1];
        GameSessionMapper mapper = GameSessionMapper.getInstance();
        if (!mapper.mapping.containsKey(roomId)) {
            mapper.mapping.put(roomId, new ArrayList<>());

        }
        mapper.mapping.get(roomId).add(session);
    }

    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage jsonTextMessage) throws Exception {
        session.sendMessage(new TextMessage("test"));
    }
}

