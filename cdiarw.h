/*
 * Copyright (C) 2018 Cyber Defense Institute, Inc.	
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 
 * as published by the Free Software Foundation.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CDI_ARW_H
#define _CDI_ARW_H



struct arw_param {
	void *src;
	void *dest;
	ssize_t len;
};

#define CDIARW_WRITE 0x4018cd10
#define CDIARW_READ 0x8018cd10

#endif