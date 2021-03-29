#include "context.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {

    float vertices[] = {
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,     // top right, green
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,    // bottom right, white
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // bottom left, green
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,    // top left, white
    };
        uint32_t indices[] = {  // note that we start from 0! 점의 인덱스 표현
        0, 1, 3,                // first triangle
        1, 2, 3,                // second triangle
    };

    m_vertexLayout = VertexLayout::Create();                                       //array obj -> 버퍼 obj 순서중요

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices, sizeof(float) * 24);                             //용도는 "STATIC | DYNAMIC | STREAM", "DRAW | COPY | READ"의 조합
  	
    //m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);   //점의 위치 묘사 n | size | type | normailzed | stride | offset
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);
   
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
	  
    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
} 

void Context::CreateCircle(float radius, float in_radius, int segment, float st_angle, float ed_angle, float R, float G, float B){
   std::vector<float> vertices;
   std::vector<uint32_t> indices;

    const float pi = 3.141592f;                     //파이정의
    for (int i = 0; i< segment; i++){
        float angle = (360.f / segment * i) * pi / 180.0f;
        angle = angle + st_angle *pi /180.0f;                //시작각 설정

        if(angle > ed_angle * pi /180.0f)                    //끝각 도달시 for 종료
            break;
            
        float x = cosf(angle) * radius;                      //  큰원 x좌표
        float y = sinf(angle) * radius;                      //  큰원 y좌표
        float a = cosf(angle) * in_radius;                      //  작은원 x좌표
        float b = sinf(angle) * in_radius;                      //  작은원 y좌표

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        vertices.push_back(a);
        vertices.push_back(b);
        vertices.push_back(0.0f);

        
    }
    int count=0;
    for(int i = 1; i <= vertices.size()/3 ; i++){
            if(i== segment*2 -1)
            {
                indices.push_back(count+2);
                indices.push_back(0);
                indices.push_back(1);
            }
            else if(i == segment*2-2)
            {
                indices.push_back(count+3);
                indices.push_back(1);
                indices.push_back(count+4);
            }
            indices.push_back(count);
            indices.push_back(count+1);
            indices.push_back(count+2);
        
        if(i/3!=0)
        count ++;
    }

    m_vertexLayout = VertexLayout::Create();                                        //array obj -> 버퍼 obj 순서중요

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());          //용도는 "STATIC | DYNAMIC | STREAM", "DRAW | COPY | READ"의 조합
  	
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);      //점의 위치 묘사 n | size | type | normailzed | stride | offset

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();

    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, R, G, B, 1.0f);
}