#include "../include/iostream"

namespace NMSP {

#ifndef KLIB

/******************************************************************************
 ******************************************************************************/

// These are global objects. They get properly initialised by the constructor
// of ios_base::Init. If they are default contructed first, it is harmless.
ifstream cin;
ofstream cout;
ofstream cerr;

/******************************************************************************
 ******************************************************************************/

#endif /* KLIB not defined */
} // end NMSP namespace

