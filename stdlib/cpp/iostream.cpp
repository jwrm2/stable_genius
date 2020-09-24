#include "../include/fstream"

namespace NMSP {

#ifndef KLIB

/******************************************************************************
 ******************************************************************************/

// These are global objects. They get properly initialised by the constructor
// of ios_base::Init. We can't rely on the order of initialisation. If these
// got constructed as streams first, they would be overwritten in the Init
// constructor, meaning the destructor of the old object wouldn't be called,
// which would leak a file buffer. More seriously, if constructors were called
// here after the Init constructor, we might default construct over the standard
// streams, which then wouldn't be connected.
// To get round this, we play a trick on the compiler. We give the objects here
// a fake type, with the correct size and alignment, meaning they will get zero
// initialised when the .bss section is loaded, and no global constructors are
// called. We disable the compiler type checking by not including the iostream
// header (which declares these objects). We still need the fstream header to
// get the object sizes.
alignas(ifstream) char cin[sizeof(ifstream) / sizeof(char)];
alignas(ofstream) char cout[sizeof(ofstream) / sizeof(char)];
alignas(ofstream) char cerr[sizeof(ofstream) / sizeof(char)];

/******************************************************************************
 ******************************************************************************/

#endif /* KLIB not defined */
} // end NMSP namespace

