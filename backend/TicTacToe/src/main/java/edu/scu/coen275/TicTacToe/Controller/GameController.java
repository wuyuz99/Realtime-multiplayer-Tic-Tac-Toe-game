package edu.scu.coen275.TicTacToe.Controller;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.util.JSONPObject;
import edu.scu.coen275.TicTacToe.dto.ConnectRequest;
import edu.scu.coen275.TicTacToe.exception.InvalidGameException;
import edu.scu.coen275.TicTacToe.exception.NotFoundException;
import edu.scu.coen275.TicTacToe.exception.RoomFullException;
import edu.scu.coen275.TicTacToe.model.Game;
import edu.scu.coen275.TicTacToe.model.GamePlay;
import edu.scu.coen275.TicTacToe.model.Player;
import edu.scu.coen275.TicTacToe.service.GameService;
import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.ResponseEntity;
import org.springframework.messaging.core.AbstractDestinationResolvingMessagingTemplate;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;

import java.io.IOException;
import java.util.List;

@RestController
@Slf4j
@AllArgsConstructor
@RequestMapping("/game")
public class GameController {
    private final GameService gameService;
    @PostMapping("/host")
    public ResponseEntity<Game> start(@RequestBody Player player) {
        log.info("host room request: {}", player);
        return ResponseEntity.ok(gameService.createGame(player));
    }

    @PostMapping("/join")
    public ResponseEntity<Game> join(@RequestBody ConnectRequest request) throws RoomFullException, NotFoundException, IOException {
        log.info("joining to room: {}", request);
        Game game = gameService.connectToGame(request.getPlayer(), request.getGameId());
        sendGame(game);
        game = gameService.gameReady(request.getGameId());
        return ResponseEntity.ok(game);
    }

    @PostMapping("/gameplay")
    public ResponseEntity<Game> gamePlay(@RequestBody GamePlay request) throws InvalidGameException, NotFoundException, IOException {
        log.info("gamePlay: {}", request);
        Game game = gameService.gamePlay(request);
        sendGame(game);
        return ResponseEntity.ok(game);
    }

    private void sendGame(Game game) throws IOException {
        ObjectMapper JSONMapper = new ObjectMapper();
        String roomId = game.getGameId();
        GameSessionMapper mapper = GameSessionMapper.getInstance();
        if (mapper.mapping.containsKey(roomId)) {
            List<WebSocketSession> sessions = mapper.mapping.get(roomId);
            String jsonString = JSONMapper.writeValueAsString(game);
            for (WebSocketSession session: sessions) {
                session.sendMessage(new TextMessage(jsonString));
            }
        }
    }
}
