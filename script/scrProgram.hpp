#pragma once

#include "../base/pgBase.hpp"

namespace rage
{
    class scrProgram : public pgBase
    {
    public:
        
    int GetNumRefs() const
    {
        const int32_t* numRefs = reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(this) + 92);
        return static_cast<int>(*numRefs);
    }

    void TestForMismatchedGlobals(uint32_t globalId, uint32_t referenceId)
    {
        if (globalId == 0 || referenceId == 0)
            return;
            const bool allowMismatchedGlobals = m_allow_mismatched_globals;
        if (!allowMismatchedGlobals) {
            // mismatched globals detected
            return;
        }
    }

    void AddRef() 
    {
        constexpr int32_t refCountOffset = 92;
        int32_t* refCount = reinterpret_cast<int32_t*>(reinterpret_cast<char*>(this) + refCountOffset);
        ++(*refCount);
        static int32_t globalVar = 0;
        ++globalVar;
    }

    int32_t GetMetadata(uint32_t index) 
    {
        constexpr int32_t metadataOffset = 136;
        int32_t* metadataPtr = reinterpret_cast<int32_t*>(reinterpret_cast<char*>(this) + metadataOffset);
            if (*metadataPtr == 0)
                return 0;
        int32_t* metadataArray = reinterpret_cast<int32_t*>(*metadataPtr);
            if (metadataArray[index] == 0)
                return 0;
        int32_t* metadataEntry = reinterpret_cast<int32_t*>(reinterpret_cast<char*>(*metadataPtr) + metadataArray[index]);
            return *metadataEntry;
    }

    int32_t GetMetadataDebug(uint32_t index) 
    {
        constexpr int32_t metadataOffset = 152;
        int32_t* metadataDebugPtr = reinterpret_cast<int32_t*>(reinterpret_cast<char*>(this) + metadataOffset);
        if (*metadataDebugPtr == 0)
            return 0;
        int32_t* metadataDebugArray = reinterpret_cast<int32_t*>(*metadataDebugPtr);
        if (metadataDebugArray[index] == 0)
            return 0;
        int32_t* metadataDebugEntry = reinterpret_cast<int32_t*>(*metadataDebugPtr + metadataDebugArray[index]);
        return *metadataDebugEntry;
    }

    bool IsProgramPoolAboveWarningThreshold() 
    {
        constexpr int32_t progHashSize = 32;
        void** ProgHashData = reinterpret_cast<void**>(&m_prog_hash);
        int32_t counter = 0;
        int32_t i = 0;
        while (i < progHashSize) 
        {
            void* elementAddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(*ProgHashData) + i);
            uint8_t elementValue = *reinterpret_cast<uint8_t*>(elementAddr);
            if (elementValue != 0) 
            {
                i += elementValue;
                ++counter;
            } 
            else 
            {
                ++i;
            }
        }
        return counter > 128;
    }
   
    void InitClass()
    {
        constexpr size_t globalBlockSize = sizeof(uintptr_t);
        constexpr size_t globalBlockCount = 4;
        uintptr_t* globalBlockPtr = reinterpret_cast<uintptr_t*>(&m_global_block_can_be_accessed);
        for (size_t i = 0; i < globalBlockCount * globalBlockSize / sizeof(uintptr_t); ++i) {
            globalBlockPtr[i] = 0;
        }
    }
    
    void InitialiseAllGlobalBlockCanBeAccessedFlags() 
    {
        constexpr uintptr_t globalBlockSize = sizeof(uintptr_t);
        constexpr uintptr_t globalBlockCount = 4;
        bool* globalBlockPtr = &m_global_block_can_be_accessed;
        for (uintptr_t i = 0; i < globalBlockCount; ++i) {
            uintptr_t* blockStartAddr = reinterpret_cast<uintptr_t*>(globalBlockPtr + i * globalBlockSize);
            for (uintptr_t j = 0; j < globalBlockSize / sizeof(uintptr_t); ++j) {
                blockStartAddr[j] = 0;
            }
        }
    }

    bool GetGlobalBlockCanBeAccessed(int index)
    {
        constexpr int maxBlocks = 64;
        if (index >= maxBlocks)
            return false;
        bool* globalBlockPtr = &m_global_block_can_be_accessed;
        bool blockValue = globalBlockPtr[index];
        return blockValue;
    }

    void SetGlobalBlockCanBeAccessed(int index, bool value)
    {
        constexpr int maxBlocks = 64;
        if (index < 0 || index >= maxBlocks)
            return;
        bool* globalBlockPtr = &m_global_block_can_be_accessed;
        globalBlockPtr[index] = value;
    }

    private:
        uint8_t** m_script_table;                   // 0x0000
        uint32_t m_globals_hash;                    // 0x0008
        uint32_t m_size_of_opcode;                  // 0x000C
        uint32_t m_size_of_arguments;               // 0x0010
        uint32_t m_size_of_static_data;             // 0x0014
        uint32_t m_size_of_global_data;             // 0x0018
        uint32_t m_size_of_native;                  // 0x001C
        uint32_t* m_static_data;                    // 0x0020
        uint32_t** m_global_data_table;             // 0x0028
        uint64_t* m_natives;                        // 0x0030
        uint32_t m_number_of_procedures;            // 0x0038
        const char** m_procedure_names;             // 0x003C
        uint32_t m_hash_code;                       // 0x0044
        uint32_t m_reference_count;                 // 0x0048
        const char* m_script_name;                  // 0x004C
        const char** m_string_heaps;                // 0x0050
        uint32_t m_size_of_string_heap;             // 0x0058
        uint8_t m_has_program_break_points[11];     // 0x005C
        char pad_0060[4];                           // 0x0060
        char pad_0064[4];                           // 0x0064
        char pad_0068[4];                           // 0x0068
    protected:
        static bool m_allow_mismatched_globals;
        static bool m_global_block_can_be_accessed;
        void* m_prog_hash;
    };
}