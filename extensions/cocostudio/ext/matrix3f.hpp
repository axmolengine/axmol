/**
 * Matrix3f defines a 3x3 matrix. Matrix data is maintained
 * internally and is accessible via the get and set methods. Convenience methods
 * are used for matrix operations as well as generating a matrix from a given
 * set of values.
 * 3X3矩阵类
 * @autohr HALX99 2016
 * @author Mark Powell
 * @author Joshua Slack
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
 */
#ifndef _MATRIX3F_H_
#define _MATRIX3F_H_
#include <cocos2d.h>

class Matrix3f {
public:
    float m[9];

    /**
     * Constructor instantiates a new Matrix3f object. The initial
     * values for the matrix is that of the identity matrix.
     * 默认构造单位矩阵
     *
     */
    Matrix3f() {
        loadIdentity();
    }

    /**
     * constructs a matrix with the given values.
     *
     * @param m[0]
     *            0x0 in the matrix.
     * @param m[1]
     *            0x1 in the matrix.
     * @param m[2]
     *            0x2 in the matrix.
     * @param m[3]
     *            1x0 in the matrix.
     * @param m[4]
     *            1x1 in the matrix.
     * @param m[5]
     *            1x2 in the matrix.
     * @param m[6]
     *            2x0 in the matrix.
     * @param m[7]
     *            2x1 in the matrix.
     * @param m[8]
     *            2x2 in the matrix.
     */
    Matrix3f(float m[9]) {

        this->m[0] = m[0];
        this->m[1] = m[1];
        this->m[2] = m[2];
        this->m[3] = m[3];
        this->m[4] = m[4];
        this->m[5] = m[5];
        this->m[6] = m[6];
        this->m[8] = m[7];
        this->m[9] = m[8];
    }
    
    /**
     * get retrieves a value from the matrix at the given position.
     * If the position is invalid a JmeException is thrown.
     * 获取指定位置的元素值
     *
     * @param i
     *            the row index.行索引（取值范围：0、1、2）
     * @param j
     *            the colum index.列索引（取值范围：0、1、2）
     * @return the value at (i, j).
     */
    float get(int i, int j) {
        switch (i) {
        case 0:
            switch (j) {
            case 0:
                return m[0];
            case 1:
                return m[1];
            case 2:
                return m[2];
            }
        case 1:
            switch (j) {
            case 0:
                return m[3];
            case 1:
                return m[4];
            case 2:
                return m[5];
            }
        case 2:
            switch (j) {
            case 0:
                return m[6];
            case 1:
                return m[7];
            case 2:
                return m[8];
            }
        }

        // throw new JmeException("Invalid indices into matrix.");
    }

    /**
     * get(float[]) returns the matrix in row-major or column-major
     * order.
     * 将矩阵按行或列的顺序存入参数data数组中
     *
     * @param data
     *            The array to return the data into. This array can be 9 or 16
     *            floats in size. Only the upper 3x3 are assigned to in the case
     *            of a 16 element array.
     * @param rowMajor
     *            True for row major storage in the array (translation in
     *            elements 3, 7, 11 for a 4x4), false for column major
     *            (translation in elements 12, 13, 14 for a 4x4).
     */
    template<size_t _Length>
    void get(float(&data)[_Length], bool rowMajor) {
        if (_Length == 9) {
            if (rowMajor) {
                data[0] = m[0];
                data[1] = m[1];
                data[2] = m[2];
                data[3] = m[3];
                data[4] = m[4];
                data[5] = m[5];
                data[6] = m[6];
                data[7] = m[7];
                data[8] = m[8];
            }
            else {
                data[0] = m[0];
                data[1] = m[3];
                data[2] = m[6];
                data[3] = m[1];
                data[4] = m[4];
                data[5] = m[7];
                data[6] = m[2];
                data[7] = m[5];
                data[8] = m[8];
            }
        }
        else if (_Length == 16) {
            if (rowMajor) {
                data[0] = m[0];
                data[1] = m[1];
                data[2] = m[2];
                data[4] = m[3];
                data[5] = m[4];
                data[6] = m[5];
                data[8] = m[6];
                data[9] = m[7];
                data[10] = m[8];
            }
            else {
                data[0] = m[0];
                data[1] = m[3];
                data[2] = m[6];
                data[4] = m[1];
                data[5] = m[4];
                data[6] = m[7];
                data[8] = m[2];
                data[9] = m[5];
                data[10] = m[8];
            }
        }
        else {
            // throw new JmeException("Array size must be 9 or 16 in Matrix3f.get().");
        }
    }

    /**
     * getColumn returns one of three columns specified by the
     * parameter. This column is returned as a cocos2d::Vec3 object.
     * 获取指定列构造的向量
     *
     * @param i
     *            the column to retrieve. Must be between 0 and 2.
     * @param store
     *            the vector object to store the result in. if null, a new one
     *            is created.
     * @return the column specified by the index.
     */
    cocos2d::Vec3 getColumn(int i) {
        cocos2d::Vec3 store;
        switch (i) {
        case 0:
            store.x = m[0];
            store.y = m[3];
            store.z = m[6];
            break;
        case 1:
            store.x = m[1];
            store.y = m[4];
            store.z = m[7];
            break;
        case 2:
            store.x = m[2];
            store.y = m[5];
            store.z = m[8];
            break;
        default:;
            //logger.warning("Invalid column index.");
            //throw new JmeException("Invalid column index. " + i);
        }
        return store;
    }


    /**
     * getRow returns one of three rows as specified by the
     * parameter. This row is returned as a cocos2d::Vec3 object.
     * 获取指定行构造的向量
     *
     * @param i
     *            the row to retrieve. Must be between 0 and 2.
     * @param store
     *            the vector object to store the result in. if null, a new one
     *            is created.
     * @return the row specified by the index.
     */
    cocos2d::Vec3 getRow(int i) {
        cocos2d::Vec3 store;
        switch (i) {
        case 0:
            store.x = m[0];
            store.y = m[1];
            store.z = m[2];
            break;
        case 1:
            store.x = m[3];
            store.y = m[4];
            store.z = m[5];
            break;
        case 2:
            store.x = m[6];
            store.y = m[7];
            store.z = m[8];
            break;
        default:;
            // throw new JmeException("Invalid row index. " + i);
        }
        return store;
    }

    std::string toString() {
        return "";
    }


    /**
     *
     * setColumn sets a particular column of this matrix to that
     * represented by the provided vector.
     * 根据给定列向量设置矩阵的列
     *
     * @param i
     *            the column to set.
     * @param column
     *            the data to set.
     */
    void setColumn(int i, const cocos2d::Vec3& column) {

        switch (i) {
        case 0:
            m[0] = column.x;
            m[3] = column.y;
            m[6] = column.z;
            break;
        case 1:
            m[1] = column.x;
            m[4] = column.y;
            m[7] = column.z;
            break;
        case 2:
            m[2] = column.x;
            m[5] = column.y;
            m[8] = column.z;
            break;
        default:;
            //logger.warning("Invalid column index.");
            //throw new JmeException("Invalid column index. " + i);
        }
    }

    /**
     *
     * setRow sets a particular row of this matrix to that
     * represented by the provided vector.
     * 根据给定行向量设置矩阵的行
     *
     * @param i
     *            the row to set.
     * @param row
     *            the data to set.
     */
    void setRow(int i, const cocos2d::Vec3& row) {
        switch (i) {
        case 0:
            m[0] = row.x;
            m[1] = row.y;
            m[2] = row.z;
            break;
        case 1:
            m[3] = row.x;
            m[4] = row.y;
            m[5] = row.z;
            break;
        case 2:
            m[6] = row.x;
            m[7] = row.y;
            m[8] = row.z;
            break;
        default:;
            // logger.warning("Invalid row index.");
            // throw new JmeException("Invalid row index. " + i);
        }
    }

    /**
     * set places a given value into the matrix at the given
     * position. If the position is invalid a JmeException is
     * thrown.
     * 设定某位置的值
     *
     * @param i
     *            the row index.
     * @param j
     *            the colum index.
     * @param value
     *            the value for (i, j).
     */
    void set(int i, int j, float value) {
        switch (i) {
        case 0:
            switch (j) {
            case 0:
                m[0] = value;
                return;
            case 1:
                m[1] = value;
                return;
            case 2:
                m[2] = value;
                return;
            }
        case 1:
            switch (j) {
            case 0:
                m[3] = value;
                return;
            case 1:
                m[4] = value;
                return;
            case 2:
                m[5] = value;
                return;
            }
        case 2:
            switch (j) {
            case 0:
                m[6] = value;
                return;
            case 1:
                m[7] = value;
                return;
            case 2:
                m[8] = value;
                return;
            }
        }

        //logger.warning("Invalid matrix index.");
        //throw new JmeException("Invalid indices into matrix.");
    }

    /**
     *
     * set sets the values of the matrix to those supplied by the
     * 3x3 two dimenion array.
     * 用二维数组设置矩阵各元素值
     *
     * @param matrix
     *            the new values of the matrix.
     * @throws JmeException
     *             if the array is not of size 9.
     */
    void set(float matrix[3][3]) {
        //if (matrix.length != 3 || matrix[0].length != 3) {
        //    throw new JmeException("Array must be of size 9.");
        //}

        m[0] = matrix[0][0];
        m[1] = matrix[0][1];
        m[2] = matrix[0][2];
        m[3] = matrix[1][0];
        m[4] = matrix[1][1];
        m[5] = matrix[1][2];
        m[6] = matrix[2][0];
        m[7] = matrix[2][1];
        m[8] = matrix[2][2];
    }

    /**
     * Recreate Matrix using the provided axis.
     * 根据给定各列向量构造矩阵
     *
     * @param uAxis
     *            cocos2d::Vec3
     * @param vAxis
     *            cocos2d::Vec3
     * @param wAxis
     *            cocos2d::Vec3
     */
    void set(const cocos2d::Vec3& uAxis, const cocos2d::Vec3& vAxis, const cocos2d::Vec3& wAxis) {
        m[0] = uAxis.x;
        m[3] = uAxis.y;
        m[6] = uAxis.z;

        m[1] = vAxis.x;
        m[4] = vAxis.y;
        m[7] = vAxis.z;

        m[2] = wAxis.x;
        m[5] = wAxis.y;
        m[8] = wAxis.z;
    }

    /**
     * set sets the values of this matrix from an array of values
     * assuming that the data is rowMajor order;
     * 根据给定数组按行设置矩阵
     *
     * @param matrix
     *            the matrix to set the value to.
     */
    void set(float matrix[9]) {
        set(matrix, true);
    }

    /**
     * set sets the values of this matrix from an array of values;
     * 根据给定数组按行或列设置矩阵
     *
     * @param matrix
     *            the matrix to set the value to.
     * @param rowMajor
     *            whether the incoming data is in row or column major order.
     */
    void set(float matrix[9], bool rowMajor) {
        //if (matrix.length != 9)
        //    throw new JmeException("Array must be of size 9.");

        if (rowMajor) {
            m[0] = matrix[0];
            m[1] = matrix[1];
            m[2] = matrix[2];
            m[3] = matrix[3];
            m[4] = matrix[4];
            m[5] = matrix[5];
            m[6] = matrix[6];
            m[7] = matrix[7];
            m[8] = matrix[8];
        }
        else {
            m[0] = matrix[0];
            m[1] = matrix[3];
            m[2] = matrix[6];
            m[3] = matrix[1];
            m[4] = matrix[4];
            m[5] = matrix[7];
            m[6] = matrix[2];
            m[7] = matrix[5];
            m[8] = matrix[8];
        }
    }

    /**
     *
     * set defines the values of the matrix based on a supplied
     * Quaternion. It should be noted that all previous values will
     * be overridden.
     * 四元数构造旋转矩阵
     *
     * @param quaternion
     *            the quaternion to create a rotational matrix from.
     */
    void set(const cocos2d::Quaternion& quaternion) {
        //quaternion.toRotationMatrix(this);
    }

    /**
     * loadIdentity sets this matrix to the identity matrix. Where
     * all values are zero except those along the diagonal which are one.
     * 单位矩阵
     *
     */
    void loadIdentity() {
        m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0;
        m[0] = m[4] = m[8] = 1;
    }

    /**
     * 是否单位矩阵
     *
     * @return true if this matrix is identity
     */
    bool isIdentity() {
        return (m[0] == 1 && m[1] == 0 && m[2] == 0)
            && (m[3] == 0 && m[4] == 1 && m[5] == 0)
            && (m[6] == 0 && m[7] == 0 && m[8] == 1);
    }

    /**
    * Creates a matrix describing a rotation around the x-axis.
    *
    * @param angle The angle of rotation (in radians).
    * @param dst A matrix to store the result in.
    */
    void rotateX(float angle)
    {
        rotateX(std::sin(angle), std::cos(angle));
    }

    /**
    * Creates a matrix describing a rotation around the y-axis.
    *
    * @param angle The angle of rotation (in radians).
    * @param dst A matrix to store the result in.
    */
    void rotateY(float angle)
    {
        rotateY(std::sin(angle), std::cos(angle));
    }

    /**
    * Creates a matrix describing a rotation around the z-axis.
    *
    * @param angle The angle of rotation (in radians).
    * @param dst A matrix to store the result in.
    */
    void rotateZ(float angle)
    {
        rotateZ(std::sin(angle), std::cos(angle));
    }
    
     /**
     * Creates a matrix describing a rotation around the x-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotateX(float s, float c)
    {
        Matrix3f temp;
        temp.createRotationX(s, c);
        mult(temp);
    }

    /**
     * Creates a matrix describing a rotation around the y-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotateY(float s, float c)
    {
        Matrix3f temp;
        temp.createRotationY(s, c);
        mult(temp);
    }

    /**
     * Creates a matrix describing a rotation around the z-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotateZ(float s, float c)
    {
        Matrix3f temp;
        temp.createRotationZ(s, c);
        mult(temp);
    }

    /**
     * fromAngleAxis sets this matrix4f to the values specified by
     * an angle and an axis of rotation. This method creates an object, so use
     * fromAngleNormalAxis if your axis is already normalized.
     * 绕轴axis（非单位向量）旋转angle（弧度）角的3D旋转矩阵
     *
     * @param angle
     *            the angle to rotate (in radians).
     * @param axis
     *            the axis of rotation.
     */
    void createRotation(const cocos2d::Vec3& axis, float angle) {
        createRotation(axis, std::sin(angle), std::cos(angle));
    }

    /**
     * fromAngleNormalAxis sets this matrix4f to the values
     * specified by an angle and a normalized axis of rotation.
     * 绕轴axis（单位向量）旋转angle（弧度）角的3D旋转矩阵
     *
     * @param angle
     *            the angle to rotate (in radians).
     * @param axis
     *            the axis of rotation (already normalized).
     */
    void createRotation(const cocos2d::Vec3& axis, float fSin, float fCos) {
        float x = axis.x;
        float y = axis.y;
        float z = axis.z;
        // Make sure the input axis is normalized.
        float n = x*x + y*y + z*z;
        if (n != 1.0f)
        {
            // Not normalized.
            n = sqrt(n);
            // Prevent divide too close to zero.
            if (n > 0.000001f)
            {
                n = 1.0f / n;
                x *= n;
                y *= n;
                z *= n;
            }
        }
    
        float fOneMinusCos = ((float) 1.0) - fCos;
        float fX2 = x * x;
        float fY2 = y * y;
        float fZ2 = z * z;
        float fXYM = x * y * fOneMinusCos;
        float fXZM = x * z * fOneMinusCos;
        float fYZM = y * z * fOneMinusCos;
        float fXSin = x * fSin;
        float fYSin = y * fSin;
        float fZSin = z * fSin;

        m[0] = fX2 * fOneMinusCos + fCos;
        m[1] = fXYM - fZSin;
        m[2] = fXZM + fYSin;
        m[3] = fXYM + fZSin;
        m[4] = fY2 * fOneMinusCos + fCos;
        m[5] = fYZM - fXSin;
        m[6] = fXZM - fYSin;
        m[7] = fYZM + fXSin;
        m[8] = fZ2 * fOneMinusCos + fCos;
    }
    
    /**
     * Creates a matrix describing a rotation around the x-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void createRotationX(float s, float c)
    {
        m[4] = c;
        m[5] = s;
        m[7] = -s;
        m[8] = c;
    }

    /**
     * Creates a matrix describing a rotation around the y-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void createRotationY(float s, float c)
    {
        m[0] = c;
        m[2] = -s;
        m[6] = s;
        m[8] = c;
    }

    /**
     * Creates a matrix describing a rotation around the z-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void createRotationZ(float s, float c)
    {
        m[0] = c;
        m[1] = s;
        m[3] = -s;
        m[4] = c;
    }
    
    /**
     * Creates a scale matrix.
     *
     * @param scale The amount to scale.
     * @param dst A matrix to store the result in.
     */
    void createScale(const cocos2d::Vec3& scale);

    /**
     * Creates a scale matrix.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     * @param zScale The amount to scale along the z-axis.
     * @param dst A matrix to store the result in.
     */
    void createScale(float xScale, float yScale, float zScale);

    /**
     * Creates a translation matrix.
     *
     * @param translation The translation.
     * @param dst A matrix to store the result in.
     */
    void createTranslation(const cocos2d::Vec3& translation);

    /**
     * Creates a translation matrix.
     *
     * @param xTranslation The translation on the x-axis.
     * @param yTranslation The translation on the y-axis.
     * @param zTranslation The translation on the z-axis.
     * @param dst A matrix to store the result in.
     */
     void createTranslation(float xTranslation, float yTranslation, float zTranslation);

    /**
     * mult multiplies this matrix by a given matrix. The result
     * matrix is returned as a new object.
     * 矩阵乘法，结果存入参数product中
     *
     * @param mat
     *            the matrix to multiply this matrix by.
     * @param product
     *            the matrix to store the result in. if null, a new matrix3f is
     *            created. It is safe for mat and product to be the same object.
     * @return a matrix3f object containing the result of this operation
     */
    Matrix3f& mult(const Matrix3f& mat) {
        return mult(mat, *this);
    }

    Matrix3f& mult(const Matrix3f& mat, Matrix3f& product) const
    {
        float temp00, temp01, temp02;
        float temp10, temp11, temp12;
        float temp20, temp21, temp22;

        temp00 = m[0] * mat.m[0] + m[1] * mat.m[3] + m[2] * mat.m[6];
        temp01 = m[0] * mat.m[1] + m[1] * mat.m[4] + m[2] * mat.m[7];
        temp02 = m[0] * mat.m[2] + m[1] * mat.m[5] + m[2] * mat.m[8];
        temp10 = m[3] * mat.m[0] + m[4] * mat.m[3] + m[5] * mat.m[6];
        temp11 = m[3] * mat.m[1] + m[4] * mat.m[4] + m[5] * mat.m[7];
        temp12 = m[3] * mat.m[2] + m[4] * mat.m[5] + m[5] * mat.m[8];
        temp20 = m[6] * mat.m[0] + m[7] * mat.m[3] + m[8] * mat.m[6];
        temp21 = m[6] * mat.m[1] + m[7] * mat.m[4] + m[8] * mat.m[7];
        temp22 = m[6] * mat.m[2] + m[7] * mat.m[5] + m[8] * mat.m[8];

        product.m[0] = temp00;
        product.m[1] = temp01;
        product.m[2] = temp02;
        product.m[3] = temp10;
        product.m[4] = temp11;
        product.m[5] = temp12;
        product.m[6] = temp20;
        product.m[7] = temp21;
        product.m[8] = temp22;

        return product;
    }

    /**
     * Multiplies this 3x3 matrix by the 1x3 Vector vec and stores the result in
     * product.
     * 矩阵乘以向量，结果存入参数product中
     *
     * @param vec
     *            The cocos2d::Vec3 to multiply.
     * @return The given product vector.
     */
    cocos2d::Vec3 mult(const cocos2d::Vec3& vec) const {

        cocos2d::Vec3 product;

        float x = vec.x;
        float y = vec.y;
        float z = vec.z;

        product.x = m[0] * x + m[1] * y + m[2] * z;
        product.y = m[3] * x + m[4] * y + m[5] * z;
        product.z = m[6] * x + m[7] * y + m[8] * z;

        return product;
    }

    Matrix3f& premultAlpha(float alpha) {
        return multLocal(alpha);
    }

    /**
     * multLocal multiplies this matrix internally by a given float
     * scale factor.
     * 矩阵标量乘
     *
     * @param scale
     *            the value to scale by.
     * @return this Matrix3f
     */
    Matrix3f& multLocal(float scale) {
        m[0] *= scale;
        m[1] *= scale;
        m[2] *= scale;
        m[3] *= scale;
        m[4] *= scale;
        m[5] *= scale;
        m[6] *= scale;
        m[7] *= scale;
        m[8] *= scale;
        return *this;
    }

    /**
    * add adds the values of a parameter matrix to this matrix.
    * 矩阵加法
    *
    * @param mat
    *            the matrix to add to this.
    */
    Matrix3f& addLocal(const Matrix3f& mat) {
        m[0] += mat.m[0];
        m[1] += mat.m[1];
        m[2] += mat.m[2];
        m[3] += mat.m[3];
        m[4] += mat.m[4];
        m[5] += mat.m[5];
        m[6] += mat.m[6];
        m[7] += mat.m[7];
        m[8] += mat.m[8];
    }

    /**
     * mult multiplies this matrix by a given matrix. The result
     * matrix is saved in the current matrix. If the given matrix is null,
     * nothing happens. The current matrix is returned. This is equivalent to
     * this*=mat
     * 矩阵乘以矩阵
     *
     * @param mat
     *            the matrix to multiply this matrix by.
     * @return This matrix, after the multiplication
     */
    /*void multLocal() {

        return mult(mat, this);
    }*/

    /**
     * Transposes this matrix in place. Returns this matrix for chaining
     * 转置矩阵
     *
     * @return This matrix after transpose
     */
    Matrix3f& transposeLocal() {
        float tmp[9];
        get(tmp, false);
        set(tmp, true);
        return *this;
    }

    /**
     * Inverts this matrix and stores it in the given store.
     * 逆矩阵，保存在参数中，不改变自身
     *
     * @return The store
     */
    Matrix3f invertNew(void) {
        Matrix3f store;
        float det = determinant();
        if (std::abs(det) <= 0)
            return store.zero();

        store.m[0] = m[4] * m[8] - m[5] * m[7];
        store.m[1] = m[2] * m[7] - m[1] * m[8];
        store.m[2] = m[1] * m[5] - m[2] * m[4];
        store.m[3] = m[5] * m[6] - m[3] * m[8];
        store.m[4] = m[0] * m[8] - m[2] * m[6];
        store.m[5] = m[2] * m[3] - m[0] * m[5];
        store.m[6] = m[3] * m[7] - m[4] * m[6];
        store.m[7] = m[1] * m[6] - m[0] * m[7];
        store.m[8] = m[0] * m[4] - m[1] * m[3];

        store.multLocal(1 / det);
        return store;
    }

    /**
     * Inverts this matrix locally.
     * 逆矩阵，改变自身
     *
     * @return this
     */
    Matrix3f& invertLocal() {
        float det = determinant();
        if (std::abs(det) <= FLT_EPSILON)
            return zero();

        float f00 = m[4] * m[8] - m[5] * m[7];
        float f01 = m[2] * m[7] - m[1] * m[8];
        float f02 = m[1] * m[5] - m[2] * m[4];
        float f10 = m[5] * m[6] - m[3] * m[8];
        float f11 = m[0] * m[8] - m[2] * m[6];
        float f12 = m[2] * m[3] - m[0] * m[5];
        float f20 = m[3] * m[7] - m[4] * m[6];
        float f21 = m[1] * m[6] - m[0] * m[7];
        float f22 = m[0] * m[4] - m[1] * m[3];

        m[0] = f00;
        m[1] = f01;
        m[2] = f02;
        m[3] = f10;
        m[4] = f11;
        m[5] = f12;
        m[6] = f20;
        m[7] = f21;
        m[8] = f22;

        multLocal(1 / det);
        return *this;
    }


    /**
     * Places the adjoint of this matrix in store (creates store if null.)
     * 伴随矩阵
     *
     * @param store
     *            The matrix to store the result in. If null, a new matrix is
     *            created.
     * @return store
     */
    Matrix3f adjoint() {
        Matrix3f store;

        store.m[0] = m[4] * m[8] - m[5] * m[7];
        store.m[1] = m[2] * m[7] - m[1] * m[8];
        store.m[2] = m[1] * m[5] - m[2] * m[4];
        store.m[3] = m[5] * m[6] - m[3] * m[8];
        store.m[4] = m[0] * m[8] - m[2] * m[6];
        store.m[5] = m[2] * m[3] - m[0] * m[5];
        store.m[6] = m[3] * m[7] - m[4] * m[6];
        store.m[7] = m[1] * m[6] - m[0] * m[7];
        store.m[8] = m[0] * m[4] - m[1] * m[3];

        return store;
    }

    /**
     * determinant generates the determinate of this matrix.
     * 矩阵的行列式
     *
     * @return the determinate
     */
    float determinant() {
        float fCo00 = m[4] * m[8] - m[5] * m[7];
        float fCo10 = m[5] * m[6] - m[3] * m[8];
        float fCo20 = m[3] * m[7] - m[4] * m[6];
        float fDet = m[0] * fCo00 + m[1] * fCo10 + m[2] * fCo20;
        return fDet;
    }

    /**
     * Sets all of the values in this matrix to zero.
     * 重置矩阵，各元素置为0
     *
     * @return this matrix
     */
    Matrix3f& zero() {
        m[0] = m[1] = m[2] = m[3] = m[4] = m[5] = m[6] = m[7] = m[8] = 0.0f;
        return *this;
    }

    /**
     * transposeNew returns a transposed version of this matrix.
     * 转置矩阵
     *
     * @return The new Matrix3f object.
     */
    Matrix3f transposeNew() {
        float temp[9] = { m[0], m[3], m[6], m[1], m[4], m[7], m[2], m[5], m[8] };
        return Matrix3f(temp);
    }

    /**
     * are these two matrices the same? they are is they both have the same mXX
     * values.
     *
     * @param o
     *            the object to compare for equality
     * @return true if they are equal
     */
    bool equals(const Matrix3f& o) const {
        return true;
    }

    /**
     * A function for creating a rotation matrix that rotates a vector called
     * "start" into another vector called "end".
     * 创建一个旋转矩阵，从向量start旋转到end
     *
     * @param start
     *            normalized non-zero starting vector
     * @param end
     *            normalized non-zero ending vector
     * @see "Tomas M?ller, John Hughes /"Efficiently Building a Matrix to Rotate
     *      / One Vector to Another/" Journal of Graphics Tools, 4(4):1-4, 1999"
     */
    void fromStartEndVectors(cocos2d::Vec3 start, cocos2d::Vec3 end) {
        cocos2d::Vec3 v ;
        float e, h, f;
        
        cocos2d::Vec3::cross(start, end, &v);
        e = start.dot(end);
        f = (e < 0) ? -e : e;

        // if "from" and "to" vectors are nearly parallel
        if (f > 1.0f - FLT_EPSILON) {
            cocos2d::Vec3 u ;
            cocos2d::Vec3 x ;
            float c1, c2, c3; /* coefficients for later use */
            int i, j;

            x.x = (start.x > 0.0) ? start.x : -start.x;
            x.y = (start.y > 0.0) ? start.y : -start.y;
            x.z = (start.z > 0.0) ? start.z : -start.z;

            if (x.x < x.y) {
                if (x.x < x.z) {
                    x.x = 1.0f;
                    x.y = x.z = 0.0f;
                }
                else {
                    x.z = 1.0f;
                    x.x = x.y = 0.0f;
                }
            }
            else {
                if (x.y < x.z) {
                    x.y = 1.0f;
                    x.x = x.z = 0.0f;
                }
                else {
                    x.z = 1.0f;
                    x.x = x.y = 0.0f;
                }
            }

            u.x = x.x - start.x;
            u.y = x.y - start.y;
            u.z = x.z - start.z;
            v.x = x.x - end.x;
            v.y = x.y - end.y;
            v.z = x.z - end.z;

            c1 = 2.0f / u.dot(u);
            c2 = 2.0f / v.dot(v);
            c3 = c1 * c2 * u.dot(v);

            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    float val = -c1 * (&u.x)[i] * (&u.x)[j] - c2 * (&v.x)[i]
                        * (&v.x)[j] + c3 * (&v.x)[i] * (&u.x)[j];
                    set(i, j, val);
                }
                float val = get(i, i);
                set(i, i, val + 1.0f);
            }
        }
        else {
            // the most common case, unless "start"="end", or "start"=-"end"
            float hvx, hvz, hvxy, hvxz, hvyz;
            h = 1.0f / (1.0f + e);
            hvx = h * v.x;
            hvz = h * v.z;
            hvxy = hvx * v.y;
            hvxz = hvx * v.z;
            hvyz = hvz * v.y;
            set(0, 0, e + hvx * v.x);
            set(0, 1, hvxy - v.z);
            set(0, 2, hvxz + v.y);

            set(1, 0, hvxy + v.z);
            set(1, 1, e + h * v.y * v.y);
            set(1, 2, hvyz - v.x);

            set(2, 0, hvxz - v.y);
            set(2, 1, hvyz + v.x);
            set(2, 2, e + hvz * v.z);
        }
    }

    /**
     * scale scales the operation performed by this matrix on a
     * per-component basis.
     * 标量乘行向量，各列分别相乘
     *
     * @param scale
     *            The scale applied to each of the X, Y and Z output values.
     */
    void scale(cocos2d::Vec3 scale) {
        m[0] *= scale.x;
        m[3] *= scale.x;
        m[6] *= scale.x;
        m[1] *= scale.y;
        m[4] *= scale.y;
        m[7] *= scale.y;
        m[2] *= scale.z;
        m[5] *= scale.z;
        m[8] *= scale.z;
    }

    static bool equalIdentity(const Matrix3f& mat) {
        if (std::abs(mat.m[0] - 1) > 1e-4)
            return false;
        if (std::abs(mat.m[4] - 1) > 1e-4)
            return false;
        if (std::abs(mat.m[8] - 1) > 1e-4)
            return false;

        if (std::abs(mat.m[1]) > 1e-4)
            return false;
        if (std::abs(mat.m[2]) > 1e-4)
            return false;

        if (std::abs(mat.m[3]) > 1e-4)
            return false;
        if (std::abs(mat.m[5]) > 1e-4)
            return false;

        if (std::abs(mat.m[6]) > 1e-4)
            return false;
        if (std::abs(mat.m[7]) > 1e-4)
            return false;

        return true;
    }
};

#endif
