#include "Rect.h"

bool Rect::MouseInRect(Mouse& mouse)
{
    float normMouseX = (mouse.GetMouseCoord().X / window->GetSize().GetWidth()) * 2.0f - 1.0f;
    float normMouseY = 1.0f - (mouse.GetMouseCoord().Y / window->GetSize().GetHeight()) * 2.0f;

    return (normMouseX >= vertex1.X && normMouseX <= vertex2.X &&
        normMouseY >= vertex1.Y && normMouseY <= vertex2.Y);
}

void Rect::MathPos(Coord& vertex1, Coord& vertex2)
{
    this->vertex1 = vertex1;
	this->vertex2 = vertex2;

    size = Size(vertex2.X - vertex1.X, vertex2.Y - vertex1.Y);

    float centerX_GL = float(vertex1.X + vertex2.X) / 2.0f;
    float centerY_GL = float(vertex1.Y + vertex2.Y) / 2.0f;

    pos = Coord(((centerX_GL + 1.0f) / 2.0f) * (float)window->GetSize().GetWidth(),
        ((1.0f - (centerY_GL + 1.0f) / 2.0f) * (float)window->GetSize().GetHeight()));
}

void Rect::MathPos(Coord& pos)
{
    this->pos = pos;

    float glCenterX = window->PixelToGLX(pos.X);
    float glCenterY = window->PixelToGLY(pos.Y);

    float glWidth = (float)(size.GetWidth()) / (float)window->GetSize().GetWidth() * 2.0f;
    float glHeight = (float)size.GetHeight() / (float)window->GetSize().GetHeight() * 2.0f;

    float halfWidth = glWidth / 2.0f;
    float halfHeight = glHeight / 2.0f;

    vertex1.X = glCenterX - halfWidth;
    vertex1.Y = glCenterY - halfHeight;
    vertex2.X = glCenterX + halfWidth;
    vertex2.Y = glCenterY + halfHeight;
}

Rect::Rect()
{
    title = (char*)"Undefined";
    window = NULL;

    collision = nullptr;
    material = nullptr;

    OnMouseHover = OnMouseOver = nullptr;
    OnCollisionEnterHandler = nullptr;
    OnMouseClick = nullptr;

    moveDirection = Directions::DOWN;
}

Rect::Rect(
    const char* title,
    Window& window, Coord
    pos, Size size, Color color, 
    Directions moveDirection
)
{
    copyStr(title, this->title);

    this->window = &window;
    this->size = size;
    this->color = baseColor = color;

    MathPos(pos);

    OnMouseHover = OnMouseOver = nullptr;
    OnCollisionEnterHandler = nullptr;
    OnMouseClick = nullptr;

    material = new BaseFigureMaterial();
    material->SetShader(
        new Shader(
            title, 
            "Dodge/shaders/Test/vertex.vs", 
            "Dodge/shaders/Test/fragment.frag"
        )
    );
    material->SetDiffuse(color);
    material->SetDiffuseMap(new Image());

    this->moveDirection = moveDirection;

    collision = nullptr;
}

Rect::Rect(
    const char* title, 
    Window& window, 
    Coord vertex1, Coord vertex2, 
    Color color, Directions moveDirection
)
{
    copyStr(title, this->title);

    this->window = &window;
    this->color = baseColor = color;

    MathPos(vertex1, vertex2);

    OnMouseHover = OnMouseOver = nullptr;
    OnCollisionEnterHandler = nullptr;
    OnMouseClick = nullptr;

    material = new BaseFigureMaterial();
    material->SetShader(
        new Shader(
            title,
            "Dodge/shaders/Figures/vertex.vs",
            "Dodge/shaders/Figures/fragment.frag"
        )
    );

    this->moveDirection = moveDirection;

    collision = nullptr;
}

Rect::Rect(
    const char* title, 
    Window& window, 
    Coord vertex1, Coord vertex2, 
    Coord textureVertex1, Coord textureVertex2, 
    Color color, Directions moveDirection
)
{
    copyStr(title, this->title);

    this->window = &window;
    this->color = baseColor = color;

    MathPos(vertex1, vertex2);

    OnMouseHover = OnMouseOver = nullptr;
    OnCollisionEnterHandler = nullptr;
    OnMouseClick = nullptr;

    material = new BaseFigureMaterial();
    material->SetShader(
        new Shader(
            title,
            "Dodge/shaders/Test/vertex.vs",
            "Dodge/shaders/Test/fragment.frag"
        )
    );
    material->SetDiffuseMapVerticies({
        textureVertex1, textureVertex2
    });

    this->moveDirection = moveDirection;

    collision = nullptr;
}

void Rect::Update()
{
    Draw();
}

void Rect::Draw()
{
    material->Use(this);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    const bool isHasDiffuseVertexs = material->GetDiffuseMapVerticies().size() >= 2 && material->GetDiffuseMap() != nullptr;
    const Coord& textCoord1 = isHasDiffuseVertexs ? material->GetDiffuseMapVerticies()[0] : Coord(0, 0);
	const Coord& textCoord2 = isHasDiffuseVertexs ? material->GetDiffuseMapVerticies()[1] : Coord(1, 1);

    float vertices[] = {
        // positions         // colors
         vertex1.X, vertex1.Y, 0.0f,  textCoord1.X, textCoord1.Y,
         vertex2.X, vertex1.Y, 0.0f,  textCoord2.X, textCoord1.Y,
         vertex2.X, vertex2.Y, 0.0f,  textCoord2.X, textCoord2.Y,
		 vertex1.X, vertex2.Y, 0.0f,  textCoord1.X, textCoord2.Y
    };

    unsigned int VBO, VAO;

    glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//diffuse
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	glDrawArrays(GL_QUADS, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
    glPopAttrib();
    material->Disable(this);
}

std::vector<float> Rect::GetVerticesByDirection(Rect& rect, Directions moveDirection, bool returnTexCoords)
{
    Coord vertex1 = rect.GetVertices()[0];
    Coord vertex2 = rect.GetVertices()[1];

    const bool isHasDiffuseVertexs = rect.material->GetDiffuseMapVerticies().size() >= 2 && rect.material->GetDiffuseMap() != nullptr;
    const Coord& textCoord1 = isHasDiffuseVertexs ? rect.material->GetDiffuseMapVerticies()[0] : Coord(0, 0);
    const Coord& textCoord2 = isHasDiffuseVertexs ? rect.material->GetDiffuseMapVerticies()[1] : Coord(1, 1);

    if (moveDirection == Directions::UP) {
        return returnTexCoords ? std::vector<float> {
            (float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
                (float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
                (float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
                (float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
        }
        : std::vector<float>{
            (float)vertex2.X, (float)vertex2.Y, 0.0f, 
                (float)vertex1.X, (float)vertex2.Y, 0.0f, 
                (float)vertex1.X, (float)vertex1.Y, 0.0f, 
                (float)vertex2.X, (float)vertex1.Y, 0.0f
        };
    }

    if (moveDirection == Directions::DOWN) {
        return returnTexCoords ? std::vector<float> {
            (float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
                (float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
                (float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
                (float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
        }
        : std::vector<float>{
			(float)vertex1.X, (float)vertex1.Y, 0.0f, 
				(float)vertex2.X, (float)vertex1.Y, 0.0f, 
				(float)vertex2.X, (float)vertex2.Y, 0.0f, 
				(float)vertex1.X, (float)vertex2.Y, 0.0f
		};
    }

    if (moveDirection == Directions::LEFT) {
        return returnTexCoords 
                ? std::vector<float> {
                (float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
                (float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
                (float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
                (float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
            }
        :
            std::vector<float>{
                (float)vertex1.X, (float)vertex1.Y, 0.0f,
                (float)vertex1.X, (float)vertex2.Y, 0.0f,
                (float)vertex2.X, (float)vertex2.Y, 0.0f,
                (float)vertex2.X, (float)vertex1.Y, 0.0f,
        };
    }

    if (moveDirection == Directions::RIGHT) {
        return returnTexCoords ? std::vector<float> {
            (float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
                (float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
                (float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
                (float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
        }
        : std::vector<float>{
            (float)vertex2.X, (float)vertex2.Y, 0.0f, 
                (float)vertex2.X, (float)vertex1.Y, 0.0f, 
                (float)vertex1.X, (float)vertex1.Y, 0.0f, 
                (float)vertex1.X, (float)vertex2.Y, 0.0f, 
        };
    }
}

void Rect::RotateToDirection(Directions direction)
{
    // ������� ��������� � ����������� �� �������� � �������� �����������
    if (moveDirection == direction) return; // ��� � ������ �����������

    // ���������� ���� ��������
    if ((moveDirection == Directions::RIGHT && direction == Directions::LEFT) ||
        (moveDirection == Directions::LEFT && direction == Directions::RIGHT) ||
        (moveDirection == Directions::UP && direction == Directions::DOWN) ||
        (moveDirection == Directions::DOWN && direction == Directions::UP)) {
        // �������� �� 180 ��������
        std::swap(vertex1.X, vertex2.X);
        std::swap(vertex1.Y, vertex2.Y);
    }
    else if ((moveDirection == Directions::RIGHT && direction == Directions::UP) ||
        (moveDirection == Directions::UP && direction == Directions::LEFT) ||
        (moveDirection == Directions::LEFT && direction == Directions::DOWN) ||
        (moveDirection == Directions::DOWN && direction == Directions::RIGHT)) {
        // ������� �� 90 �������� ������ ������� �������
        float centerX = (vertex1.X + vertex2.X) / 2;
        float centerY = (vertex1.Y + vertex2.Y) / 2;

        float tempX = vertex1.X - centerX;
        float tempY = vertex1.Y - centerY;

        vertex1.X = -tempY + centerX;
        vertex1.Y = tempX + centerY;

        tempX = vertex2.X - centerX;
        tempY = vertex2.Y - centerY;

        vertex2.X = -tempY + centerX;
        vertex2.Y = tempX + centerY;
    }
    else if ((moveDirection == Directions::RIGHT && direction == Directions::DOWN) ||
        (moveDirection == Directions::DOWN && direction == Directions::LEFT) ||
        (moveDirection == Directions::LEFT && direction == Directions::UP) ||
        (moveDirection == Directions::UP && direction == Directions::RIGHT)) {
        // ������� �� 90 �������� �� ������� �������
        float centerX = (vertex1.X + vertex2.X) / 2;
        float centerY = (vertex1.Y + vertex2.Y) / 2;

        float tempX = vertex1.X - centerX;
        float tempY = vertex1.Y - centerY;

        vertex1.X = tempY + centerX;
        vertex1.Y = -tempX + centerY;

        tempX = vertex2.X - centerX;
        tempY = vertex2.Y - centerY;

        vertex2.X = tempY + centerX;
        vertex2.Y = -tempX + centerY;
    }

    // ��������� �����������
    moveDirection = direction;
}

Coord Rect::GetPos()
{
    return pos;
}

Size Rect::GetSize()
{
    return size;
}

bool Rect::MouseHover(Mouse& mouse)
{
    if (mouse.IsEqual()) {
        return false;
    }

    const bool isHover = MouseInRect(mouse);

    if (isHover && OnMouseHover) {
        OnMouseHover(this, window->GetWindow());
    }

    if (!isHover && OnMouseOver) {
        OnMouseOver(this, window->GetWindow());
    }

    return isHover;
}

bool Rect::MouseClick(Mouse& mouse)
{
    if (!mouse.isClick() || !MouseInRect(mouse)) {
        return false;
    }

    if (OnMouseClick) {
        OnMouseClick(this, window->GetWindow());
    }
    return true;
}

bool Rect::CollisionEnter(IGameObject& gameObject)
{
    if (!collision || collision == nullptr) {
        return false;
    }

    bool isEnter = collision->IsCollisionEnter(&gameObject);

    if (isEnter && OnCollisionEnterHandler) {
        OnCollisionEnterHandler(this, &gameObject, window->GetWindow());
    }
}

Window* Rect::GetWindow()
{
    return window;
}

void Rect::SetColor(Color color)
{
    this->color = color;
}

Color Rect::GetColor()
{
    return color;
}

Color Rect::GetBaseColor()
{
    return baseColor;
}

std::vector<Coord> Rect::GetVertices()
{
    return { vertex1, vertex2 };
}

void Rect::SetPos(std::vector<Coord> vertices)
{
    MathPos(vertices[0], vertices[1]);
}

void Rect::SetPos(Coord pos)
{
    MathPos(pos);
}

void Rect::SetMaterial(Material* material)
{
	this->material = material;
}

Material* Rect::GetMaterial()
{
    return material;
}

void Rect::SetCollision(ICollision* collision)
{
    this->collision = collision;
}

ICollision* Rect::GetCollision()
{
    return collision;
}

const char* Rect::GetTitle()
{
    return title;
}

void Rect::SetTitle(const char* title)
{
    copyStr(title, this->title);
}

void Rect::SetMoveDirection(Directions moveDirection)
{
    this->moveDirection = moveDirection;
}

Directions Rect::GetMoveDirection()
{
    return moveDirection;
}

const bool Rect::IsMouseOverlap()
{
    return MouseInRect(window->GetMouse());
}

void Rect::HookMouseHover(MouseHoverHandler handler)
{
    OnMouseHover = handler;
}

void Rect::HookMouseOver(MouseHoverHandler handler)
{
    OnMouseOver = handler;
}

void Rect::HookMouseClick(MouseClickHandler handler)
{
    OnMouseClick = handler;
}

void Rect::HookOnCollisionEnter(OnCollisionEnter handler)
{
    this->OnCollisionEnterHandler = handler;
}

bool Rect::operator==(const Rect& other) const
{
    return window == other.window && pos == other.pos && 
        vertex1 == other.vertex1 && vertex2 == other.vertex2 && size == other.size &&
        color == other.color && baseColor == other.baseColor && material == other.material &&
        !strcmp(title, other.title);
}

bool Rect::operator!=(const Rect& other) const
{
    return !(*this == other);
}

Rect& Rect::operator=(const Rect& other)
{
    if (this == &other) {
        return *this;
    }

    this->window = other.window;
    this->pos = other.pos;

    this->vertex1 = other.vertex1;
    this->vertex2 = other.vertex2;

    this->size = other.size;

    this->color = other.color;
    this->baseColor = other.baseColor;

    this->OnMouseHover = other.OnMouseHover;
    this->OnMouseOver = other.OnMouseOver;
    this->OnMouseClick = other.OnMouseClick;

    this->material = other.material;

    copyStr(other.title, this->title);

    return *this;
}
