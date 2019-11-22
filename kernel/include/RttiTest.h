#ifndef RTTITEST_H
#define RTTITEST_H

struct RttiBase { virtual ~RttiBase(); };

struct RttiChildA : virtual public RttiBase {};

struct RttiChildB : virtual public RttiBase {};

struct RttiDiamond : public RttiChildA, public RttiChildB {};

void rtti_test();

#endif /* RTTITEST_H */
