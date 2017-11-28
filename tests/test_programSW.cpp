#include "test_common.h"

#include <string>

#include "../Context.h"
#include "../Program.h"
#include "../SW/SW.h"

class ProgramTest : public ren::Context {
public:
    ProgramTest() {
        ren::Context::Init(1, 1);
    }
};

VSHADER vshader(VS_IN, VS_OUT) {}

FSHADER fshader(FS_IN, FS_OUT) {}

void test_program() {
    {
        // Create program
        ProgramTest test;

        ren::eProgLoadStatus status;
        ren::Attribute _attrs[] = { {} };
        ren::Uniform _unifs[] = { {} };
        ren::ProgramRef p = test.LoadProgramSW("constant", nullptr, nullptr, 0, _attrs, _unifs, &status);

        assert(p);
        assert(status == ren::ProgSetToDefault);

        {
            ren::Program *pp = p.get();

            assert(pp != nullptr);
            assert(std::string(pp->name()) == "constant");
            assert(pp->prog_id() == 0); // default value
            assert(!pp->ready());
        }

        ren::Uniform unifs[] = { {"unif1", 0, SW_FLOAT, 1}, {"unif2", 1, SW_VEC3, 1}, {} };
        ren::Attribute attrs[] = { {"attr1", 0, -1, 1}, {"attr2", 1, -1, 1}, {} };

        test.LoadProgramSW("constant", (void*)vshader, (void*)fshader, 0, attrs, unifs, &status);

        assert(status == ren::ProgCreatedFromData);

        ren::Program *pp = p.get();

        assert(pp != nullptr);
        assert(std::string(pp->name()) == "constant");

        assert(std::string(pp->uniform(0).name) == "unif1");
        assert(pp->uniform(0).loc == 0);
        assert(std::string(pp->uniform(1).name) == "unif2");
        assert(pp->uniform(1).loc == 1);

        assert(std::string(pp->attribute(0).name) == "attr1");
        assert(pp->attribute(0).loc == 0);
        assert(std::string(pp->attribute(1).name) == "attr2");
        assert(pp->attribute(1).loc == 1);

        assert(pp->ready());
    }
}
