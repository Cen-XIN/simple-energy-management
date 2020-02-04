/*
 * node.h
 *
 *  Created on: Jan 30, 2020
 *      Author: cenxin
 */

#ifndef NODE_H_
#define NODE_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "simple_m.h"

#define IS_TASK     0
#define IS_ALIVE    1

#define FULL_ACTIVE 1
#define SEMI_ACTIVE 0


using namespace omnetpp;

class Node : public cSimpleModule
{
private:
//    long numSent;
//    long numReceived;
    int mode;           // Current mode
    int neighborNum;    // Nodes adjacent to it

public:
    long energyConsumption;
    int chkMsgNum;      // CHK msg received during this round
    int count;          // know when to change mode

protected:
    // Basics
    virtual void initialize() override;
    // Message generator
    virtual SimpleMsg *generateTaskMsg();
    virtual SimpleMsg *generateChkAliveMsg(int dest);
    // Message sender
    virtual void sendChkAliveMsg();
    virtual void forwardMsgRandomly(SimpleMsg *msg);
    // Message processing
    virtual void handleMessage(cMessage *msg) override;
    // Auxiliary methods
    virtual void refreshDisplay() const override;
    void switchMode();

public:
    // Power management related
    void consumeBasicEnergy(int mode);
    void consumeExtraEnergy(int type);
    void consumeEnergy(int mode, int type);
    void switchByFreq(bool flag, int msgType); // Switch the mode if the node haven't been used after 5 rounds
};


#endif /* NODE_H_ */
