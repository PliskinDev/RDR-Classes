#pragma once

#include <cstdint>

namespace rage
{
    class pgBase
    {
    public:
        virtual ~pgBase() = default;
        virtual bool MapContainsPointer(void* ptr) { return false; }
        virtual void InitClass() = 0;
        virtual bool Validate() { return false; }
        virtual void SetHandleIndex(uint32_t index) = 0;
        virtual void ShutdownClass() = 0;
        virtual uint32_t GetHandleIndex() { return 0; }
        bool HasPageMap() const 
        {
            const uintptr_t* pageMap = *reinterpret_cast<const uintptr_t* const*>(reinterpret_cast<const char*>(this) + 8);
            return pageMap && (*reinterpret_cast<const uint8_t*>(pageMap + 11) != 0);
        }

        uintptr_t LookupMetaData(uint32_t index) const 
        {
            const uintptr_t* dataPtr = *reinterpret_cast<const uintptr_t* const*>(reinterpret_cast<const char*>(this) + 8);
            if (!dataPtr) {
                return 0;
            }
            uint8_t metaData = *reinterpret_cast<const uint8_t*>(dataPtr + 11);
            if (metaData) {
                return reinterpret_cast<uintptr_t>(dataPtr);
            }
            uintptr_t currentPtr = reinterpret_cast<uintptr_t>(dataPtr);
            while (currentPtr) {
                uint32_t currentSize = *reinterpret_cast<const uint32_t*>(currentPtr + 8);
                if (currentSize == index) {
                    return currentPtr;
                }
                currentPtr = *reinterpret_cast<const uintptr_t*>(currentPtr);
            }
            return 0;
        }
    };
}