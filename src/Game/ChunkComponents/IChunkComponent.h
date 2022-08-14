//
// Created by rellik on 8/14/22.
//

#ifndef JABG_ICHUNKCOMPONENT_H
#define JABG_ICHUNKCOMPONENT_H


class IChunkComponent
{
public:
			virtual void MakeDirty() {};
			virtual bool IsDirty() const {return false;};
};


#endif //JABG_ICHUNKCOMPONENT_H
