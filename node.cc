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
    count = 0;
    energyConsumption = 0;
    chkMsgNum = 0;

    mode = FULL_ACTIVE;
    neighborNum = gateSize("gate");

    WATCH(mode);
    WATCH(energyConsumption);

    sendChkAliveMsg();

    if (getIndex() == 0) {// Send initial message from node 0
        // Boot the process scheduling the initial message as a self-message.
        SimpleMsg *msg = generateTaskMsg();
//        numSent++;
//        scheduleAt(0.0, msg);
        forwardMsgRandomly(msg);
    }
}


void Node::handleMessage(cMessage *msg)
{
    SimpleMsg *ttmsg = check_and_cast<SimpleMsg *>(msg);

    int msgType = ttmsg->getType();

    switch(msgType) {
    case IS_ALIVE:
        EV << "[ARRIVE] CHK-ALIVE: from node["<< ttmsg->getSource()<<"] is confirmed\n";
        chkMsgNum++;
        delete ttmsg;
        switchByFreq(true, msgType);
        //===============================
//        if (chkMsgNum == neighborNum) {
//            consumeEnergy(mode, msgType);
//            chkMsgNum = 0;
//
//            count++;
//            if (count >= 3 && mode != SEMI_ACTIVE) {
//                switchMode();
//            }
//
//            sendChkAliveMsg();
//        }
        //================================
        break;
    case IS_TASK:
        switchByFreq(true, msgType);
        //================================
//        if (mode == SEMI_ACTIVE) {
//            switchMode();
//        }
//        consumeEnergy(mode, msgType);
        //================================
        if (ttmsg->getDestination() == getIndex()) {
            // Message arrived.
            int hopcount = ttmsg->getHopCount();

            EV << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
    //        numReceived++;
            delete ttmsg;
            bubble("ARRIVED, starting new message!");

            // Generate another one.
            EV << "Generating another message: ";
            SimpleMsg *newmsg = generateTaskMsg();
            EV << newmsg << endl;
            forwardMsgRandomly(newmsg);
    //        numSent++;
        }
        else {
            // We need to forward the message.
            forwardMsgRandomly(ttmsg);
        }
        break;
    }
}


SimpleMsg *Node::generateTaskMsg()
{
    int src = getIndex();  // our module index
    int n = getVectorSize();  // module vector size
    int dest = intuniform(0, n-1);
    int type = IS_TASK;

    while (dest == src)
        dest = intuniform(0, n-1);

    char msgname[20];
    sprintf(msgname, "Task: %d->%d", src, dest);

    // Create message object and set source and destination field.
    SimpleMsg *msg = new SimpleMsg(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    msg->setType(type);
    //powerConsumption += 2;
    return msg;
}


SimpleMsg *Node::generateChkAliveMsg(int dest)
{
    int src = getIndex();
    int type = IS_ALIVE;

    SimpleMsg *msg = new SimpleMsg();
    msg->setSource(src);
    msg->setDestination(dest); // "dest" here means the gate it will be sent, not the node index
    msg->setType(type);

    EV << "[NEW] CHK-ALIVE: ["<< msg->getSource()<<"] on gate["<< msg->getDestination() <<"] is generated\n";

    return msg;
}


void Node::sendChkAliveMsg()
{
    int src = getIndex();
    int n = gateSize("gate");

    for (int i = 0; i < n; i++) {
        SimpleMsg *chkmsg = generateChkAliveMsg(i);
        send(chkmsg, "gate$o", i);
    }
}


void Node::forwardMsgRandomly(SimpleMsg *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gateSize("gate"); // get the number of neighbors
    int k = intuniform(0, n-1);//randomly choose a neighbor gate

    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    send(msg, "gate$o", k);
}


void Node::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "State: %d Power: %ld", mode, energyConsumption);
    getDisplayString().setTagArg("t", 0, buf);
}


void Node::switchMode()
{
    if (mode == FULL_ACTIVE) {
        mode = SEMI_ACTIVE;
    } else {
        mode = FULL_ACTIVE;
        count = 0;
    }
}


void Node::consumeBasicEnergy(int mode)
{
    switch(mode) {
    case SEMI_ACTIVE:
        energyConsumption += 2;
        break;
    case FULL_ACTIVE:
        energyConsumption += 4;
        break;
    }
}


void Node::consumeExtraEnergy(int type)
{
    switch(type) {
    case IS_ALIVE:
        energyConsumption += 2;
        break;
    case IS_TASK:
        energyConsumption += 2;
        break;
    }
}

void Node::consumeEnergy(int mode, int type)
{
    switch(mode) {
    case SEMI_ACTIVE:
        switch(type) {
        case IS_ALIVE:
            energyConsumption += 1;
            break;
        case IS_TASK:
            EV << "[Warning] message is passed to a semi-activated node";
            break;
        }
        break;
    case FULL_ACTIVE:
        switch(type) {
        case IS_ALIVE:
            energyConsumption += 2;
            break;
        case IS_TASK:
            energyConsumption += 2;
            break;
        }
        break;
    }
}

void Node::switchByFreq(bool flag, int msgType)
{//TODO: simplify the handleMessage function
    if (flag == true) {
        switch(msgType) {
        case IS_ALIVE:
            if (chkMsgNum == neighborNum) {
                consumeEnergy(mode, msgType);
                chkMsgNum = 0;

                count++;
                if (count >= 3 && mode != SEMI_ACTIVE) {
                    switchMode();
                }

                sendChkAliveMsg();
            }
            break;
        case IS_TASK:
            if (mode == SEMI_ACTIVE) {
                switchMode();
            }
            consumeEnergy(mode, msgType);
            break;
        }
    } else {
        switch(msgType) {
        case IS_ALIVE:
            if (chkMsgNum == neighborNum) {
                consumeEnergy(mode, msgType);
                chkMsgNum = 0;
                sendChkAliveMsg();
            }
            break;
        case IS_TASK:
            consumeEnergy(mode, msgType);
            break;
        }
    }
}


