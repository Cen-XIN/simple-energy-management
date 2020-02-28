/*
 * node.cc
 *
 *  Created on: Jan 28, 2020
 *      Author: Cen XIN
 */

#include "node.h"


Define_Module(Node);


void Node::initialize()
{
    srand(time(NULL)); // set the random seed (simulation ONLY)

    count = 0;
    prob = 0.0;
    energyConsumption = 0.0;
    chkMsgNum = 0;

    mode = FULL_ACTIVE;
    neighborNum = gateSize("gate");
    is_here = false;

    WATCH(mode);
    WATCH(energyConsumption);

    sendChkMsg();

    if (getIndex() == 0 || getIndex() == 5 || getIndex() == 7) {// Initial messages from different node
        // Boot the process scheduling the initial message as a self-message.
        SimpleMsg *msg = genTskMsg();
        fwdMsg(msg);
    }
}


void Node::handleMessage(cMessage *msg)
{
    SimpleMsg *ttmsg = check_and_cast<SimpleMsg *>(msg);

    int msgType = ttmsg->getType();

    switch(msgType) {
    case IS_CHK:
        EV << "CHK-MSG from node["<< ttmsg->getSource()<<"] is confirmed\n";
        chkMsgNum++;
        delete ttmsg;
        switchByFreq(true, msgType);
        //switchByProb(true, msgType);
        break;
    case IS_TSK:
        switchByFreq(true, msgType);
        //switchByProb(true, msgType);
        if (ttmsg->getDestination() == getIndex()) {
            delete ttmsg;
            bubble("TSK-MSG arrived, start new TSK-MSG");
            // Generate another one.
            EV << "TSK-MSG ";
            SimpleMsg *newmsg = genTskMsg();
            EV << newmsg << " is generated\n";
            fwdMsg(newmsg);
        }
        else {
            // We need to forward the message.
            fwdMsg(ttmsg);
        }
        break;
    }
}


SimpleMsg *Node::genTskMsg()
{
    int src = getIndex();       // our module index
    int n = getVectorSize();    // module vector size
    int dest = rand() % n;      //intuniform(0, n-1);
    int type = IS_TSK;

    while (dest == src)
        dest = rand() % n;      //intuniform(0, n-1);

    char msgname[20];
    sprintf(msgname, "[%d]->[%d]", src, dest);

    // Create message object and set source and destination field.
    SimpleMsg *msg = new SimpleMsg(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    msg->setType(type);
    //powerConsumption += 2;
    return msg;
}


SimpleMsg *Node::genChkMsg(int dest)
{
    int src = getIndex();
    int type = IS_CHK;

    SimpleMsg *msg = new SimpleMsg();
    msg->setSource(src);
    msg->setDestination(dest); // "dest" here means the gate it will be sent, not the node index
    msg->setType(type);

    EV << "CHK-MSG ["<< msg->getSource()<<"] on gate["<< msg->getDestination() <<"] is generated\n";

    return msg;
}


void Node::sendChkMsg()
{
    int src = getIndex();
    int n = gateSize("gate");

    for (int i = 0; i < n; i++) {
        SimpleMsg *chkmsg = genChkMsg(i);
        send(chkmsg, "gate$o", i);
    }
}


void Node::fwdMsg(SimpleMsg *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gateSize("gate"); // get the number of neighbors
    int k = rand() % n;//intuniform(0, n-1);//randomly choose a neighbor gate

    EV << "Forward message " << msg << " on gate[" << k << "]\n";
    send(msg, "gate$o", k);
}


void Node::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "M: %d P: %.2f", mode, energyConsumption);
    getDisplayString().setTagArg("t", 0, buf);
}


void Node::switchMode()
{
    if (mode == FULL_ACTIVE) {
        mode = SEMI_ACTIVE;
        prob = 0;
    } else {
        mode = FULL_ACTIVE;
        count = 0;
        prob = 0;
    }
    switchLoseEnergy();
}


void Node::consumeEnergy(int mode, int type)
{
    switch(mode) {
    case SEMI_ACTIVE:
        switch(type) {
        case IS_CHK:
            energyConsumption += genFluctEnergy(0.5, 0.8); // CHK-MSG in SEMI-ACT: 0.5~0.8 Joules
            break;
        case IS_TSK:
            EV << "[Warning] message is passed to a semi-activated node";
            break;
        }
        break;
    case FULL_ACTIVE:
        switch(type) {
        case IS_CHK:
            energyConsumption += genFluctEnergy(1.2, 1.5); // CHK-MSG in FULL-ACT: 1.2~1.5 Joules
            break;
        case IS_TSK:
            energyConsumption += genFluctEnergy(1.5, 2.0); // TSK-MSG in FULL-ACT: 1.5~2.0 Joules
            break;
        }
        break;
    }
}


void Node::switchLoseEnergy()
{
    energyConsumption += genFluctEnergy(0.2, 0.5); // Energy lose when switching: 0.2~0.5 Joules
}


void Node::switchByFreq(bool flag, int msgType)
{
    if (flag == true) {
        switch(msgType) {
        case IS_CHK:
            if (chkMsgNum == neighborNum) {
                consumeEnergy(mode, msgType);
                chkMsgNum = 0;
                count++;
                if (count >= MAX_UNUSED && mode != SEMI_ACTIVE) {
                    switchMode();
                }
                sendChkMsg();
            }
            break;
        case IS_TSK:
            if (mode == SEMI_ACTIVE) {
                switchMode();
            }
            consumeEnergy(mode, msgType);
            break;
        }
    } else {
        switch(msgType) {
        case IS_CHK:
            if (chkMsgNum == neighborNum) {
                consumeEnergy(mode, msgType);
                chkMsgNum = 0;
                sendChkMsg();
            }
            break;
        case IS_TSK:
            consumeEnergy(mode, msgType);
            break;
        }
    }
}


void Node::switchByProb(bool flag, int msgType)
{
    if (flag == true) {
        switch(msgType) {
        case IS_CHK:
            if (chkMsgNum == neighborNum) {
                consumeEnergy(mode, msgType);
                chkMsgNum = 0;
                prob = (rand() % 100) / 100.0;
                if (prob > SWITCH_PROB) {
                    switchMode();
                }
                sendChkMsg();
            }
            break;
        case IS_TSK:
            if (mode == SEMI_ACTIVE) {
                switchMode();
            }
            consumeEnergy(mode, msgType);
            break;
        }
    } else {
        switch(msgType) {
        case IS_CHK:
            if (chkMsgNum == neighborNum) {
                consumeEnergy(mode, msgType);
                chkMsgNum = 0;
                sendChkMsg();
            }
            break;
        case IS_TSK:
            consumeEnergy(mode, msgType);
            break;
        }
    }
}


float Node::genFluctEnergy(float low, float high)
{
    return (low + (rand() % int((high - low) * 100)) / 100.0);
}


