#include "test_common.h"

#include <string>

#include "../Anim.h"
#include "../Context.h"

namespace {
    unsigned char __anim[] = {
            0x41, 0x4e, 0x49, 0x4d, 0x5f, 0x53, 0x45, 0x51, 0x55, 0x45, 0x4e, 0x00,
            0x03, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
            0x30, 0x01, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x78, 0x01, 0x00, 0x00,
            0x84, 0x00, 0x00, 0x00, 0x42, 0x6f, 0x6e, 0x65, 0x30, 0x31, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xbd, 0x93, 0x60,
            0x30, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x6f, 0x6e, 0x65,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x42, 0x6f, 0x6e, 0x65, 0x30, 0x32, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xbd, 0x93, 0x60,
            0x30, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x6f, 0x6e, 0x65,
            0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x6d, 0x61, 0x74, 0x75, 0x72, 0x65,
            0x41, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x09, 0x39, 0x00, 0x28, 0x30, 0x2e, 0x30, 0x00,
            0x30, 0x2e, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x18, 0x00, 0x00, 0x00,
            0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf3, 0x04, 0x35, 0x3f, 0xf4, 0x04, 0x35, 0x3f, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xf3, 0x04, 0x35, 0x3f, 0xf4, 0x04, 0x35, 0x3f,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x04, 0x35, 0x3f,
            0xf4, 0x04, 0x35, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf3, 0x04, 0x35, 0x3f, 0xf4, 0x04, 0x35, 0x3f, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xf3, 0x04, 0x35, 0x3f, 0xf4, 0x04, 0x35, 0x3f,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x04, 0x35, 0xbf,
            0xf4, 0x04, 0x35, 0x3f
    };
    unsigned int __anim_len = 508;
}

void test_anim() {
    {   // Load anim
        ren::Context ctx;
        ctx.Init(1, 1);
        ren::AnimSeqRef anim_ref = ctx.LoadAnimSequence("anim", __anim);

        assert(std::string(anim_ref->name()) == "ArmatureAction");
        assert(anim_ref->fps() == 24);
        assert(anim_ref->len() == 3);
        assert(anim_ref->frame_size() == 11);
        assert(anim_ref->frame_dur() == 1.0f / 24);
        assert(anim_ref->anim_dur() == 3.0f / 24);
        assert(anim_ref->num_bones() == 2);

        assert(std::string(anim_ref->bone(0)->name) == "Bone01");
        assert(std::string(anim_ref->bone(0)->parent_name) == "None");
        assert(anim_ref->bone(0)->id == 0);
        assert(anim_ref->bone(0)->offset == 0);
        assert((anim_ref->bone(0)->flags & ren::AnimHasTranslate) == 1);

        assert(std::string(anim_ref->bone(1)->name) == "Bone02");
        assert(std::string(anim_ref->bone(1)->parent_name) == "Bone01");
        assert(anim_ref->bone(1)->id == 1);
        assert(anim_ref->bone(1)->offset == 7); // 4 for rotation, 3 for translation from previous bone
        assert(anim_ref->bone(1)->flags != ren::AnimHasTranslate);

        //translation of Bone01 frame 0
        assert(anim_ref->frames()[0] == 0);
        assert(anim_ref->frames()[1] == 0);
        assert(anim_ref->frames()[2] == 0);

        // rotation of Bone01 frame 0
        assert(anim_ref->frames()[3] == 0);
        assert(anim_ref->frames()[4] == 0);
        assert(anim_ref->frames()[5] == Approx(0.7071067690849304).epsilon(0.0001));
        assert(anim_ref->frames()[6] == Approx(0.7071067690849304).epsilon(0.0001));

        // rotation of Bone02 frame 0
        assert(anim_ref->frames()[7] == 0);
        assert(anim_ref->frames()[8] == 0);
        assert(anim_ref->frames()[9] == Approx(0.7071067690849304).epsilon(0.0001));
        assert(anim_ref->frames()[10] == Approx(0.7071067690849304).epsilon(0.0001));

        //translation of Bone01 frame 1
        assert(anim_ref->frames()[11] == 0);
        assert(anim_ref->frames()[12] == 5);
        assert(anim_ref->frames()[13] == 0);

        // rotation of Bone01 frame 1
        assert(anim_ref->frames()[14] == 0);
        assert(anim_ref->frames()[15] == 0);
        assert(anim_ref->frames()[16] == Approx(0.7071067690849304).epsilon(0.0001));
        assert(anim_ref->frames()[17] == Approx(0.7071067690849304).epsilon(0.0001));

        // rotation of Bone02 frame 1
        assert(anim_ref->frames()[18] == 0);
        assert(anim_ref->frames()[19] == 0);
        assert(anim_ref->frames()[20] == Approx(0.7071067690849304).epsilon(0.0001));
        assert(anim_ref->frames()[21] == Approx(0.7071067690849304).epsilon(0.0001));

        //translation of Bone01 frame 2
        assert(anim_ref->frames()[22] == 0);
        assert(anim_ref->frames()[23] == 0);
        assert(anim_ref->frames()[24] == 0);

        // rotation of Bone01 frame 2
        assert(anim_ref->frames()[25] == 0);
        assert(anim_ref->frames()[26] == 0);
        assert(anim_ref->frames()[27] == Approx(0.7071067690849304).epsilon(0.0001));
        assert(anim_ref->frames()[28] == Approx(0.7071067690849304).epsilon(0.0001));

        // rotation of Bone02 frame 2
        assert(anim_ref->frames()[29] == 0);
        assert(anim_ref->frames()[30] == 0);
        assert(anim_ref->frames()[31] == Approx(-0.7071067690849304).epsilon(0.0001));
        assert(anim_ref->frames()[32] == Approx(0.7071067690849304).epsilon(0.0001));
    }

}
