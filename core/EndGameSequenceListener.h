#ifndef _END_GAME_SEQUENCE_LISTENER_H
#define _END_GAME_SEQUENCE_LISTENER_H

class EndGameSequenceListener {
public:
    virtual void onEndGameSequenceStart() = 0;
    virtual void onEndGameSequenceEnd() = 0;
};

#endif