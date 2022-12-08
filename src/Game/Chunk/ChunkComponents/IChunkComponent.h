#ifndef IChunkComponent_H
#define IChunkComponent_H


class IChunkComponent
{
public:
    virtual void MakeDirty() {};
    virtual bool IsDirty() const {return false;};
};


#endif //JABG_ICHUNKCOMPONENT_H
