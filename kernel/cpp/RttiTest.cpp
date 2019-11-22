#include "RttiTest.h"

#include <ostream>
#include <typeinfo>

#include "Kernel.h"
#include "Logger.h"
#include "Scheduler.h"

RttiBase::~RttiBase() {}

void rtti_test()
{
    // RTTI tests.
    klib::ostream& dest = *global_kernel->syslog()->stream();
    Kernel& k = *global_kernel;

    // Typeid
    dest << "RTTI tests\n";
    dest << "Type of Kernel is " << typeid(k).name() << '\n';
    dest << "Type of kernel Scheduler is " << typeid(k.get_scheduler()).name() << '\n';
    RoundRobin s {};
    dest << "Type of RoundRobin is " << typeid(s).name() << '\n';
    if (typeid(s) == typeid(k.get_scheduler()))
        dest << "Scheduler types match.\n";
    else
        k.syslog()->error("Scheduler types do not match.\n");
    if (typeid(s) == typeid(k))
        k.syslog()->error("Scheduler type matches Kernel type.\n");
    else
        dest << "Scheduler type does not match Kernel type.\n";

    // Pointer casts
    RoundRobin* sp1 = &s;
    dest << "Type of RoundRobin pointer is " << typeid(sp1).name() << '\n';
    Scheduler* sp2 = sp1;
    dest << "RoundRobin pointer upcast to Scheduler pointer is " << typeid(sp2).name() << '\n';
    RoundRobin* sp3 = dynamic_cast<RoundRobin*>(sp2);
    if (sp3 == nullptr)
        k.syslog()->error("Good downcast from Scheduler pointer to RoundRobin pointer returned nullptr.\n");
    else
        dest << "Good downcast from Scheduler pointer to RoundRobin pointer suceeded.\n";
    Kernel* k1 = dynamic_cast<Kernel*>(sp1);
    if (k1 == nullptr)
        dest << "Bad downcast from Scheduler pointer to Kernel pointer returned nullptr.\n";
    else
        k.syslog()->error("Bad downcast from Scheduler pointer to Kernel pointer succeeded.\n");

    // Reference casts
    RoundRobin& sr1 = s;
    dest << "Type of RoundRobin reference is " << typeid(sr1).name() << '\n';
    Scheduler& sr2 = sr1;
    dest << "RoundRobin reference upcast to Scheduler reference is " << typeid(sr2).name() << '\n';
    RoundRobin& sr3 = dynamic_cast<RoundRobin&>(sr2);
    dest << "Scheduler reference dynamic cast to RoundRobin reference is " << typeid(sr3).name() << '\n';
    try {
        dynamic_cast<Kernel&>(sr1);
        k.syslog()->error("Bad downcast from RoundRobin reference to Kernel reference succeeded.\n");
    }
    catch (klib::bad_cast&)
    {
        dest << "Bad downcast from RoundRobin reference to Kernel reference raised bad_cast exception.\n";
    }

    // Pointer casts with vmi
    {
        RttiDiamond rdi;
        RttiDiamond* rdi1 = &rdi;
        dest << "Type of RttiDiamond pointer is " << typeid(rdi1).name() << '\n';
        RttiChildA* rca1 = rdi1;
        dest << "RttiDiamond pointer upcast to RttiChildA pointer is " << typeid(rca1).name() << '\n';
        RttiChildB* rcb1 = rdi1;
        dest << "RttiDiamond pointer upcast to RttiChildB pointer is " << typeid(rcb1).name() << '\n';
        RttiBase* rb1 = rdi1;
        dest << "RttiDiamond pointer upcast to RttiBase pointer is " << typeid(rb1).name() << '\n';
        RttiDiamond* rdi2 = dynamic_cast<RttiDiamond*>(rb1);
        if (rdi2 == nullptr)
            k.syslog()->error("Good downcast from RttiBase pointer to RttiDiamond pointer returned nullptr.\n");
        else
            dest << "Good downcast from RttiBase pointer to RttiDiamond pointer suceeded.\n";
        RttiChildA* rca2 = dynamic_cast<RttiChildA*>(rb1);
        if (rca2 == nullptr)
            k.syslog()->error("Good downcast from RttiBase pointer to RttiChildA pointer returned nullptr.\n");
        else
            dest << "Good downcast from RttiBase pointer to RttiChildA pointer suceeded.\n";
        RttiChildB* rcb2 = dynamic_cast<RttiChildB*>(rb1);
        if (rcb2 == nullptr)
            k.syslog()->error("Good downcast from RttiBase pointer to RttiChildB pointer returned nullptr.\n");
        else
            dest << "Good downcast from RttiBase pointer to RttiChildB pointer suceeded.\n";
        RttiChildA* rca3 = dynamic_cast<RttiChildA*>(rcb1);
        if (rca3 == nullptr)
            k.syslog()->error("Good cross cast from RttiChildB pointer to RttiChildA pointer returned nullptr.\n");
        else
            dest << "Good cross cast from RttiChildB pointer to RttiChildA pointer suceeded.\n";
        RttiChildB* rcb3 = dynamic_cast<RttiChildB*>(rca1);
        if (rcb3 == nullptr)
            k.syslog()->error("Good cross cast from RttiChildA pointer to RttiChildB pointer returned nullptr.\n");
        else
            dest << "Good cross cast from RttiChildA pointer to RttiChildB pointer suceeded.\n";
        RttiBase rb {};
        RttiBase* rb2 = &rb;
        RttiDiamond* rdi3 = dynamic_cast<RttiDiamond*>(rb2);
        if (rdi3 == nullptr)
            dest << "Bad downcast from RttiBase pointer to RttiDiamond pointer returned nullptr.\n";
        else
            k.syslog()->error("Bad downcast from RttiBase pointer to RttiDiamond pointer suceeded.\n");
        RttiChildA* rca4 = dynamic_cast<RttiChildA*>(rb2);
        if (rca4 == nullptr)
            dest << "Bad downcast from RttiBase pointer to RttiChildA pointer returned nullptr.\n";
        else
            k.syslog()->error("Bad downcast from RttiBase pointer to RttiChildA pointer suceeded.\n");
        RttiChildB* rcb4 = dynamic_cast<RttiChildB*>(rb2);
        if (rcb4 == nullptr)
            dest << "Bad downcast from RttiBase pointer to RttiChildB pointer returned nullptr.\n";
        else
            k.syslog()->error("Bad downcast from RttiBase pointer to RttiChildB pointer suceeded.\n");
        RttiChildA rca {};
        RttiChildA* rca5 = &rca;
        RttiChildB* rcb5 = dynamic_cast<RttiChildB*>(rca5);
        if (rcb5 == nullptr)
            dest << "Bad cross cast from RttiChildA pointer to RttiChildB pointer returned nullptr.\n";
        else
            k.syslog()->error("Bad cross cast from RttiChildA pointer to RttiChildB pointer suceeded.\n");
        RttiChildB rcb {};
        RttiChildB* rcb6 = &rcb;
        RttiChildA* rca6 = dynamic_cast<RttiChildA*>(rcb6);
        if (rca6 == nullptr)
            dest << "Bad cross cast from RttiChildB pointer to RttiChildA pointer returned nullptr.\n";
        else
            k.syslog()->error("Bad cross cast from RttiChildB pointer to RttiChildA pointer suceeded.\n");
    }

    // Reference casts with vmi
    {
        RttiDiamond rdi;
        RttiDiamond& rdi1 = rdi;
        dest << "Type of RttiDiamond reference is " << typeid(rdi1).name() << '\n';
        RttiChildA& rca1 = rdi1;
        dest << "RttiDiamond reference upcast to RttiChildA reference is " << typeid(rca1).name() << '\n';
        RttiChildB& rcb1 = rdi1;
        dest << "RttiDiamond reference upcast to RttiChildB reference is " << typeid(rcb1).name() << '\n';
        RttiBase& rb1 = rdi1;
        dest << "RttiDiamond reference upcast to RttiBase reference is " << typeid(rb1).name() << '\n';
        try {
            dynamic_cast<RttiDiamond&>(rb1);
            dest << "Good downcast from RttiBase reference to RttiDiamond reference suceeded.\n";
        }
        catch (klib::bad_cast&)
        {
            k.syslog()->error("Good downcast from RttiBase reference to RttiDiamond reference raised bad_cast.\n");
        }
        try {
            dynamic_cast<RttiChildA&>(rb1);
            dest << "Good downcast from RttiBase reference to RttiChildA reference suceeded.\n";
        }
        catch (klib::bad_cast&)
        {
            k.syslog()->error("Good downcast from RttiBase reference to RttiChildA reference raised bad_cast.\n");
        }
        try {
            dynamic_cast<RttiChildB&>(rb1);
            dest << "Good downcast from RttiBase reference to RttiChildB reference suceeded.\n";
        }
        catch (klib::bad_cast&)
        {
            k.syslog()->error("Good downcast from RttiBase reference to RttiChildB reference raised bad_cast.\n");
        }
        try {
            dynamic_cast<RttiChildA&>(rcb1);
            dest << "Good cross cast from RttiChildB reference to RttiChildA reference suceeded.\n";
        }
        catch (klib::bad_cast&)
        {
            k.syslog()->error("Good cross cast from RttiChildB reference to RttiChildA reference raised bad_cast.\n");
        }
        try {
            dynamic_cast<RttiChildB&>(rca1);
            dest << "Good cross cast from RttiChildA reference to RttiChildB reference suceeded.\n";
        }
        catch (klib::bad_cast&)
        {
            k.syslog()->error("Good cross cast from RttiChildA reference to RttiChildB reference raised bad_cast.\n");
        }
        RttiBase rb {};
        RttiBase& rb2 = rb;
        try {
            dynamic_cast<RttiDiamond&>(rb2);
            k.syslog()->error("Bad downcast from RttiBase reference to RttiDiamond reference suceeded.\n");
        }
        catch (klib::bad_cast&)
        {
            dest << "Bad downcast from RttiBase reference to RttiDiamond reference raised bad_cast.\n";
        }
        try {
            dynamic_cast<RttiChildA&>(rb2);
            k.syslog()->error("Bad downcast from RttiBase reference to RttiChildA reference suceeded.\n");
        }
        catch (klib::bad_cast&)
        {
            dest << "Bad downcast from RttiBase reference to RttiChildA reference raised bad_cast.\n";
        }
/*        try {
            dynamic_cast<RttiChildB&>(rb2);
            k.syslog()->error("Bad downcast from RttiBase reference to RttiChildB reference suceeded.\n");
        }
        catch (klib::bad_cast&)
        {
            dest << "Bad downcast from RttiBase reference to RttiChildB reference raised bad_cast.\n";
        }
        RttiChildA rca {};
        RttiChildA& rca5 = rca;
        try {
            dynamic_cast<RttiChildB&>(rca5);
            k.syslog()->error("Bad cross cast from RttiChildA reference to RttiChildB reference suceeded.\n");
        }
        catch (klib::bad_cast&)
        {
            dest << "Bad cross cast from RttiChildA reference to RttiChildB reference raised bad_cast.\n";
        }
        RttiChildB rcb {};
        RttiChildB& rcb6 = rcb;
        try {
            dynamic_cast<RttiChildA&>(rcb6);
            k.syslog()->error("Bad cross cast from RttiChildB reference to RttiChildA reference suceeded.\n");
        }
        catch (klib::bad_cast&)
        {
            dest << "Bad cross cast from RttiChildB reference to RttiChildA reference raised bad_cast.\n";
        }*/
    }

    dest << "End of RTTI tests\n";
}
