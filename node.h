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
#include <stdlib.h>
#include <time.h>

#include "simple_m.h"

#define IS_TSK      0
#define IS_CHK      1

#define FULL_ACTIVE 1
#define SEMI_ACTIVE 0

#define MAX_UNUSED  1   // the node with no TSK-MSG arriving in coming intervals
#define SWITCH_PROB 0.9 // if larger than this prob, switch mode


using namespace omnetpp;

class Node : public cSimpleModule
{
private:
//    long numSent;
//    long numReceived;
    int mode;           // Current mode
    int neighborNum;    // Nodes adjacent to it
    bool is_here;        // whether TSK-MSG is on the node

public:
    float energyConsumption;
    int chkMsgNum;      // CHK-MSG received during this round
    int count;          // accumulate and switch mode
    float prob;         // randomly generate and switch mode

protected:
    // Basics
    virtual void initialize() override;
    // Message generator
    virtual SimpleMsg *genTskMsg();
    virtual SimpleMsg *genChkMsg(int dest);
    // Message sender
    virtual void sendChkMsg();
    virtual void fwdMsg(SimpleMsg *msg);
    // Message processing
    virtual void handleMessage(cMessage *msg) override;
    // Auxiliary methods
    virtual void refreshDisplay() const override;

public:
    // Power management related
    void consumeEnergy(int mode, int type);
    void switchLoseEnergy();                    // Energy source while switching
    // Simple algorithms
    void switchByFreq(bool flag, int msgType);  // Switch the mode if the node haven't been used after 5 rounds
    void switchByProb(bool flag, int msgType);  // Switch the mode with a designated probability

    float genFluctEnergy(float low, float high);// Generate a random energy consumption within a designated range
    void switchMode();
};


#endif /* NODE_H_ */
