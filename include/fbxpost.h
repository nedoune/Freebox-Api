#ifndef FBXPOST_H
#define FBXPOST_H


class fbxPost
{
    public:
        fbxPost();
        virtual ~fbxPost();

        bool Getsuccess() { return success; }
        void Setsuccess(bool val) { success = val; }
        //fbxanswer Getanswer() { return answer; }
        //void Setanswer(fbxanswer val) { answer = val; }

    protected:

    private:
        bool success;
        //fbxanswer answer;
};



#endif // FBXPOST_H
