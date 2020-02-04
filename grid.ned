//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple Node
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gate[];
}

network NodeGrid
{
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }
    submodules:
        node[9]: Node;
    connections:
        node[0].gate++ <--> Channel <--> node[1].gate++;
        node[1].gate++ <--> Channel <--> node[2].gate++;
        node[3].gate++ <--> Channel <--> node[4].gate++;
        node[4].gate++ <--> Channel <--> node[5].gate++;
        node[6].gate++ <--> Channel <--> node[7].gate++;
        node[7].gate++ <--> Channel <--> node[8].gate++;
        node[0].gate++ <--> Channel <--> node[3].gate++;
        node[3].gate++ <--> Channel <--> node[6].gate++;
        node[1].gate++ <--> Channel <--> node[4].gate++;
        node[4].gate++ <--> Channel <--> node[7].gate++;
        node[2].gate++ <--> Channel <--> node[5].gate++;
        node[5].gate++ <--> Channel <--> node[8].gate++;
}
