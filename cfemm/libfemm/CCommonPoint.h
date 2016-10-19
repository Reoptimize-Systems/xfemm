#ifndef FEMM_CCOMMONPOINT_H
#define FEMM_CCOMMONPOINT_H


namespace femm {

/**
 * @brief The CCommonPoint class
 */
class CCommonPoint
{
    public:
        CCommonPoint();

        int x;
        int y;
        int t;

        /**
         * @brief sortXY
         * Sort the fields x and y.
         * After this operation, <tt>x <= y</tt> holds true.
         */
        void sortXY();
    private:

};

}
#endif
