#include <cmath>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.h"


static float DegToRad(float angDeg)
{
    const float degToRad = M_PI * 2.0f / 360.0f;
    return angDeg * degToRad;
}

static inline float Clamp(float fValue, float fMinValue, float fMaxValue)
{
    if(fValue < fMinValue)
        return fMinValue;
    
    if(fValue > fMaxValue)
        return fMaxValue;
    
    return fValue;
}

static glm::mat3 RotateX(float angDeg)
{
    float angRad = DegToRad(angDeg);
    float cos = cosf(angRad);
    float sin = sinf(angRad);
    
    glm::mat3 theMat(1.0f);
    theMat[1].y = cos;
    theMat[2].y = -sin;
    theMat[1].z = sin;
    theMat[2].z = cos;
    return theMat;
}

static glm::mat3 RotateY(float angDeg)
{
    float angRad = DegToRad(angDeg);
    float cos = cosf(angRad);
    float sin = sinf(angRad);
    
    glm::mat3 theMat(1.0f);
    theMat[0].x = cos;
    theMat[2].x = sin;
    theMat[0].z = -sin;
    theMat[2].z = cos;
    return theMat;
}

static glm::mat3 RotateZ(float angDeg)
{
    float angRad = DegToRad(angDeg);
    float cos = cosf(angRad);
    float sin = sinf(angRad);
    
    glm::mat3 theMat(1.0f);
    theMat[0].x = cos;
    theMat[1].x = -sin;
    theMat[0].y = sin;
    theMat[1].y = cos;
    return theMat;
}

class MatrixStack
{
public:
    MatrixStack()
    : m_currMat(1.0f)
    {
    }
    
    const glm::mat4 &Top()
    {
        return m_currMat;
    }
    
    void RotateX(float fAngDeg)
    {
        m_currMat = m_currMat * glm::mat4(::RotateX(fAngDeg));
    }
    
    void RotateY(float fAngDeg)
    {
        m_currMat = m_currMat * glm::mat4(::RotateY(fAngDeg));
    }
    
    void RotateZ(float fAngDeg)
    {
        m_currMat = m_currMat * glm::mat4(::RotateZ(fAngDeg));
    }
    
    void Scale(const glm::vec3 &scaleVec)
    {
        glm::mat4 scaleMat(1.0f);
        scaleMat[0].x = scaleVec.x;
        scaleMat[1].y = scaleVec.y;
        scaleMat[2].z = scaleVec.z;
        
        m_currMat = m_currMat * scaleMat;
    }
    
    void Translate(const glm::vec3 &offsetVec)
    {
        glm::mat4 translateMat(1.0f);
        translateMat[3] = glm::vec4(offsetVec, 1.0f);
        
        m_currMat = m_currMat * translateMat;
    }
    
    void Push()
    {
        m_matrices.push(m_currMat);
    }
    
    void Pop()
    {
        m_currMat = m_matrices.top();
        m_matrices.pop();
    }
    
private:
    glm::mat4 m_currMat;
    std::stack<glm::mat4> m_matrices;
};

class Hierarchy
{
public:
    Hierarchy()
    : posBase(glm::vec3(3.0f, -5.0f, -40.0f))
    , angBase(-45.0f)
    , posBaseLeft(glm::vec3(2.0f, 0.0f, 0.0f))
    , posBaseRight(glm::vec3(-2.0f, 0.0f, 0.0f))
    , scaleBaseZ(3.0f)
    , angUpperArm(-33.75f)
    , sizeUpperArm(9.0f)
    , posLowerArm(glm::vec3(0.0f, 0.0f, 8.0f))
    , angLowerArm(146.25f)
    , lenLowerArm(5.0f)
    , widthLowerArm(1.5f)
    , posWrist(glm::vec3(0.0f, 0.0f, 5.0f))
    , angWristRoll(0.0f)
    , angWristPitch(67.5f)
    , lenWrist(2.0f)
    , widthWrist(2.0f)
    , posLeftFinger(glm::vec3(1.0f, 0.0f, 1.0f))
    , posRightFinger(glm::vec3(-1.0f, 0.0f, 1.0f))
    , angFingerOpen(180.0f)
    , lenFinger(2.0f)
    , widthFinger(0.5f)
    , angLowerFinger(45.0f)
    {}
    
    void Draw()
    {
        MatrixStack modelToCameraStack;
        
        glUseProgram(theProgram);
        glBindVertexArray(vao);
        
        modelToCameraStack.Translate(posBase);
        modelToCameraStack.RotateY(angBase);
        
        //Draw left base.
        {
            modelToCameraStack.Push();
            modelToCameraStack.Translate(posBaseLeft);
            modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
        }
        
        //Draw right base.
        {
            modelToCameraStack.Push();
            modelToCameraStack.Translate(posBaseRight);
            modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
        }
        
        //Draw main arm.
        DrawUpperArm(modelToCameraStack);
        
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
#define STANDARD_ANGLE_INCREMENT 11.25f
#define SMALL_ANGLE_INCREMENT 9.0f
    
    void AdjBase(bool bIncrement)
    {
        angBase += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angBase = fmodf(angBase, 360.0f);
    }
    
    void AdjUpperArm(bool bIncrement)
    {
        angUpperArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angUpperArm = Clamp(angUpperArm, -90.0f, 0.0f);
    }
    
    void AdjLowerArm(bool bIncrement)
    {
        angLowerArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angLowerArm = Clamp(angLowerArm, 0.0f, 146.25f);
    }
    
    void AdjWristPitch(bool bIncrement)
    {
        angWristPitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angWristPitch = Clamp(angWristPitch, 0.0f, 90.0f);
    }
    
    void AdjWristRoll(bool bIncrement)
    {
        angWristRoll += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angWristRoll = fmodf(angWristRoll, 360.0f);
    }
    
    void AdjFingerOpen(bool bIncrement)
    {
        angFingerOpen += bIncrement ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
        angFingerOpen = Clamp(angFingerOpen, 9.0f, 180.0f);
    }
    
    void WritePose()
    {
        printf("angBase:\t%f\n", angBase);
        printf("angUpperArm:\t%f\n", angUpperArm);
        printf("angLowerArm:\t%f\n", angLowerArm);
        printf("angWristPitch:\t%f\n", angWristPitch);
        printf("angWristRoll:\t%f\n", angWristRoll);
        printf("angFingerOpen:\t%f\n", angFingerOpen);
        printf("\n");
    }
    
private:
    void DrawFingers(MatrixStack &modelToCameraStack)
    {
        //Draw left finger
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posLeftFinger);
        modelToCameraStack.RotateY(angFingerOpen);
        
        modelToCameraStack.Push();
        modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
        modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();
        
        {
            //Draw left lower finger
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
            modelToCameraStack.RotateY(-angLowerFinger);
            
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
            modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
            
            modelToCameraStack.Pop();
        }
        
        modelToCameraStack.Pop();
        
        //Draw right finger
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posRightFinger);
        modelToCameraStack.RotateY(-angFingerOpen);
        
        modelToCameraStack.Push();
        modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
        modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();
        
        {
            //Draw right lower finger
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
            modelToCameraStack.RotateY(angLowerFinger);
            
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
            modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
            
            modelToCameraStack.Pop();
        }
        
        modelToCameraStack.Pop();
    }
    
    void DrawWrist(MatrixStack &modelToCameraStack)
    {
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posWrist);
        modelToCameraStack.RotateZ(angWristRoll);
        modelToCameraStack.RotateX(angWristPitch);
        
        modelToCameraStack.Push();
        modelToCameraStack.Scale(glm::vec3(widthWrist / 2.0f, widthWrist/ 2.0f, lenWrist / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();
        
        DrawFingers(modelToCameraStack);
        
        modelToCameraStack.Pop();
    }
    
    void DrawLowerArm(MatrixStack &modelToCameraStack)
    {
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posLowerArm);
        modelToCameraStack.RotateX(angLowerArm);
        
        modelToCameraStack.Push();
        modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenLowerArm / 2.0f));
        modelToCameraStack.Scale(glm::vec3(widthLowerArm / 2.0f, widthLowerArm / 2.0f, lenLowerArm / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();
        
        DrawWrist(modelToCameraStack);
        
        modelToCameraStack.Pop();
    }
    
    void DrawUpperArm(MatrixStack &modelToCameraStack)
    {
        modelToCameraStack.Push();
        modelToCameraStack.RotateX(angUpperArm);
        
        {
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, (sizeUpperArm / 2.0f) - 1.0f));
            modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, sizeUpperArm / 2.0f));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
        }
        
        DrawLowerArm(modelToCameraStack);
        
        modelToCameraStack.Pop();
    }
    
    glm::vec3		posBase;
    float			angBase;
    
    glm::vec3		posBaseLeft, posBaseRight;
    float			scaleBaseZ;
    
    float			angUpperArm;
    float			sizeUpperArm;
    
    glm::vec3		posLowerArm;
    float			angLowerArm;
    float			lenLowerArm;
    float			widthLowerArm;
    
    glm::vec3		posWrist;
    float			angWristRoll;
    float			angWristPitch;
    float			lenWrist;
    float			widthWrist;
    
    glm::vec3		posLeftFinger, posRightFinger;
    float			angFingerOpen;
    float			lenFinger;
    float			widthFinger;
    float			angLowerFinger;
};