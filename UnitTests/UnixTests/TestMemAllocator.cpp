//
// Created by steve on 4/7/17.
//
#include "../../Catch.hpp"
#include "../../src/MemAllocator.hpp"

void PlaceHolderFunction()
{
    printf("Not useful, ignore me totally");
}

TEST_CASE("Tests memory allocator for Unix platform","[MemAllocator],[MemAllocatorUnix]")
{
    std::cout << std::dec << "Process ID: " << getpid();
    PLH::MemAllocatorU allocator;
    PLH::ProtFlag R = PLH::ProtFlag::R;
    PLH::ProtFlag W = PLH::ProtFlag::W;
    PLH::ProtFlag X = PLH::ProtFlag::X;
    PLH::ProtFlag N = PLH::ProtFlag::NONE;

    REQUIRE(allocator.TranslateProtection(X) == PROT_EXEC);
    REQUIRE(allocator.TranslateProtection(W) == PROT_WRITE);
    REQUIRE(allocator.TranslateProtection(R) == PROT_READ);
    REQUIRE(allocator.TranslateProtection(N) == PROT_NONE);

    REQUIRE(allocator.TranslateProtection(X | W | R | N) == (PROT_EXEC | PROT_WRITE | PROT_READ | PROT_NONE));

    uint64_t fnAddress = (uint64_t)&PlaceHolderFunction;
    uint64_t MinAddress = fnAddress - 0x80000000;
    uint64_t MaxAddress = fnAddress + 0x80000000;


    std::cout << "fnAddress: " << std::hex << fnAddress << " Acceptable Range:" << MinAddress << "-" << MaxAddress << std::endl;

    int PageSize = getpagesize();
    std::cout << std::dec << "PageSize: " << PageSize << std::endl;

    uint8_t* Buffer = allocator.AllocateMemory(MinAddress,MaxAddress, 200, (X | W | R));
    REQUIRE(Buffer != nullptr);
    std::cout << std::hex << "Allocated At: " << (uint64_t )Buffer<< std::endl;

    //Compute some statistics about how far away allocation was
    uint64_t AllocDelta = std::abs((uint64_t)Buffer- fnAddress);
    double DeltaInGB = AllocDelta / 1000000000.0; //How far was our trampoline allocated from the target, in GB
    double DeltaPercentage = DeltaInGB / .5 * 100.0; //Allowed range is +-2GB, see in percentage how close to tolerance we were
    std::cout << "Delta:[" << DeltaInGB << " GB] Percent Tolerance Used[" << DeltaPercentage << " % out of 2GB]" << std::endl;
    REQUIRE(DeltaInGB <= 2);
}


