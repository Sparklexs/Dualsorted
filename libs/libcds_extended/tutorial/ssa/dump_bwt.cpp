/**
 *    dump_bwt.cpp
 *    Copyright (C) 2011  Francisco Claude F.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ssa.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char ** argv) {
	if(argc!=2) {
		cout << "usage: " << argv[0] << " <index-file>" << endl;
		return 0;
	}

	ifstream ssainput(argv[1]);
	ssa * _ssa = new ssa(ssainput);
	ssainput.close();
	_ssa->print_bwt();
	delete _ssa;

	return 0;
}
