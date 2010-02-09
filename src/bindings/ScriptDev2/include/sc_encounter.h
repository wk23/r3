#ifndef SC_ENCOUNTER_H
#define SC_ENCOUNTER_H

#include <iostream>

class MANGOS_DLL_DECL EncounterIndexError {
   public:
      const char* error;
      EncounterIndexError(const char* arg) : error(arg) { }
};

template <int SIZE> class MANGOS_DLL_DECL EncounterData
{
    protected:
        int storage[SIZE];
    public:

        EncounterData() {
            memset(&storage, 0, sizeof(storage));
        }

        std::string dumps() const {
            std::ostringstream dumpStream;

            for (int i = 0; i < SIZE; i++) {
                if (i) dumpStream << " ";
                dumpStream << storage[i];
            }

            return dumpStream.str();
        }

        void loads(const char* data) {
            std::istringstream loadStream(data);

            for (int i = 0; i < SIZE; i++) {
             loadStream >> storage[i];
            }
        }

        int size() const { return SIZE; };

        int& operator[](const int location) {
            if (location < 0 || location >= SIZE) throw EncounterIndexError("Invalid array access");
            else return storage[location];
        }
        int operator[](const int location) const {
            if (location < 0 || location >= SIZE) throw EncounterIndexError("Invalid array access");
            else return storage[location];
        }
};
#endif