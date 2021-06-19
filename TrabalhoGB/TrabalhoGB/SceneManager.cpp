#include "SceneManager.h"

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;
//static void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos);
//void cursorEnterCallback(GLFWwindow* window, int entered);
//void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;

	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();

}

void SceneManager::initializeGraphics()
{
	// Init GLFW
	glfwInit();

	mouse_condition = 0;

	for (int i = 0; i <= 7; i++)
	{
		xPosFil[i] = 50.0 + (100 * i);
		xPosSt[i] = 50.0 + (100 * i);
	}
	
	yPosFilt = 40.0;
	xSizeFilt = 80.0;
	ySizeFilt = 60.0;

	yPosSt = 560.0;
	xSizeSt = 60.0;
	ySizeSt = 60.0;

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Sprites", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");
	addShader("../shaders/Filtro1.vs", "../shaders/Filtro1.frag");
	addShader("../shaders/Filtro2.vs", "../shaders/Filtro2.frag");
	addShader("../shaders/Filtro3.vs", "../shaders/Filtro3.frag");
	addShader("../shaders/Filtro4.vs", "../shaders/Filtro4.frag");
	addShader("../shaders/Filtro5.vs", "../shaders/Filtro5.frag");
	addShader("../shaders/Filtro6.vs", "../shaders/Filtro6.frag");
	addShader("../shaders/Filtro7.vs", "../shaders/Filtro7.frag");

	//setup the scene -- LEMBRANDO QUE A DESCRIÇÃO DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURAÇÃO
	setupScene();

	resized = true; //para entrar no setup da câmera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	Shader* shader = new Shader(vFilename.c_str(), fFilename.c_str());
	shaders.push_back(shader);
}


void SceneManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}

void SceneManager::update()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	glfwGetCursorPos(window, &xpos, &ypos);

	//AQUI aplicaremos as transformações nos sprites	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		for (int filter = 0; filter <= 7; filter += 1)
		{
			if (TestPositionFilter(filter)){
				objects[1]->setShader(shaders[filter]);
			}
		}

		for (int sticker = 0; sticker <= 7; sticker += 1)
		{
			if (sticker == 0)
			{
				if (TestPositionSt(sticker)) {
					int position = 1;
					for (int st = 11; st <= 17; st += 1)
					{
						objects[st]->setPosition(glm::vec3(xPosSt[position], yPosSt, 0.0));
						objects[st]->setDimension(glm::vec3(xSizeSt, ySizeSt, 1.0f));
						position += 1;
					}
				}
			}
			else
			{
				if (TestPositionSt(sticker) && mouse_condition == 0) {
					mouse_condition = sticker + 10;
				}
			}
			if (mouse_condition != 0) {
				objects[mouse_condition]->setPosition(glm::vec3(xpos, height - ypos, 0.0));
				objects[mouse_condition]->setDimension(glm::vec3(xSizeSt + 40, ySizeSt + 30, 1.0f));
			}
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		mouse_condition = 0;
	}
}

void SceneManager::render()
{
	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}

	//atualiza e desenha os Sprites

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->update();
		objects[i]->draw();
	}
}

void SceneManager::run()
{
	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		update();

		//Render scene
		render();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


void SceneManager::setupScene()
{
	cout << "Digite o nome (com extensao) da imagem que deseja modificar: ";
	cin >> texture_name;
	cout << "Digite a largura da imagem: ";
	cin >> image_width;
	cout << "Digite a altura da imagem: ";
	cin >> image_height;

	 image_width = (400 * image_width) / image_height;
	 image_height = 400;

	//Mínimo: posicao e escala e ponteiro para o shader
	Sprite* obj = new Sprite;
	obj->setPosition(glm::vec3(400.0f, 300.0f, 0.0));
	obj->setDimension(glm::vec3(800.0f, 600.0f, 1.0f)); //note que depois podemos reescalar conforme tamanho da sprite
	obj->setShader(shaders[0]);
	objects.push_back(obj); //adiciona o fundo

	obj = new Sprite;
	obj->setPosition(glm::vec3(400.0f, 300.0f, 0.0));
	obj->setDimension(glm::vec3(image_width, image_height, 1.0f)); //note que depois podemos reescalar conforme tamanho da sprite
	obj->setShader(shaders[0]);
	objects.push_back(obj); //adiciona a foto original

	for (int ft = 0; ft <= 7; ft += 1)
	{
		obj = new Sprite;
		obj->setPosition(glm::vec3(xPosFil[ft], yPosFilt, 0.0));
		obj->setDimension(glm::vec3(xSizeFilt, ySizeFilt, 1.0f));
		obj->setShader(shaders[ft]);
		objects.push_back(obj); //adiciona os filtros
	}

	for (int st = 0; st <= 7; st += 1)
	{
		obj = new Sprite;
		obj->setPosition(glm::vec3(xPosSt[st], yPosSt, 0.0));
		if (st == 0) {
			obj->setDimension(glm::vec3(xSizeSt - 10, ySizeSt - 10, 1.0f));
		}
		else
		{
			obj->setDimension(glm::vec3(xSizeSt, ySizeSt, 1.0f));
		}
		obj->setShader(shaders[0]);
		objects.push_back(obj); //adiciona os stickers
	}

	//Carregamento das texturas (pode ser feito intercalado na criação)
	//Futuramente, utilizar classe de materiais e armazenar dimensoes, etc

	unsigned int texID = loadTexture("../textures/fundo.png");
	objects[0]->setTexture(texID);

	texID = loadTexture("../textures/" + texture_name);
	for (int i = 1; i <= 9; i += 1)
	{
		objects[i]->setTexture(texID);
	}

	texID = loadTexture("../textures/st_10.png");
	objects[10]->setTexture(texID);
	texID = loadTexture("../textures/st_2.png");
	objects[11]->setTexture(texID);
	texID = loadTexture("../textures/st_1.png");
	objects[12]->setTexture(texID);
	texID = loadTexture("../textures/st_4.png");
	objects[13]->setTexture(texID);
	texID = loadTexture("../textures/st_3.png");
	objects[14]->setTexture(texID);
	texID = loadTexture("../textures/st_5.png");
	objects[15]->setTexture(texID);
	texID = loadTexture("../textures/st_7.png");
	objects[16]->setTexture(texID);
	texID = loadTexture("../textures/st_9.png");
	objects[17]->setTexture(texID);

	//Definindo a janela do mundo (ortho2D)
	ortho2D[0] = 0.0f; //xMin
	ortho2D[1] = 800.0f; //xMax
	ortho2D[2] = 0.0f; //yMin
	ortho2D[3] = 600.0f; //yMax

	//Habilita transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void SceneManager::setupCamera2D() //TO DO: parametrizar aqui
{
	float zNear = -1.0, zFar = 1.0; //estão fixos porque não precisamos mudar

	projection = glm::ortho(ortho2D[0], ortho2D[1], ortho2D[2], ortho2D[3], zNear, zFar);


	//Obtendo o identificador da matriz de projeção para enviar para o shader
	shaders[0]->Use();
	GLint projLoc = glGetUniformLocation(shaders[0]->ID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	for (int i = 0; i <= 7; i += 1)
	{
		shaders[i]->Use();
		projLoc = glGetUniformLocation(shaders[i]->ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}
}

unsigned int SceneManager::loadTexture(string filename)
{
	unsigned int texture;

	// load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{

		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glActiveTexture(GL_TEXTURE0);

	return texture;
}

bool SceneManager::TestPositionFilter(int filter_number) 
{
	glfwGetCursorPos(window, &xpos, &ypos);

	if (xpos < (xPosFil[filter_number] + xSizeFilt / 2) && xpos > (xPosFil[filter_number] - xSizeFilt / 2))
	{
		if ((height - ypos) < (yPosFilt + ySizeFilt / 2) && ((height - ypos) > yPosFilt - ySizeFilt / 2))
		{
			return true;
		}
	}
	return false;
}

bool SceneManager::TestPositionSt(int sticker_number)
{
	glfwGetCursorPos(window, &xpos, &ypos);

	if (xpos < (xPosSt[sticker_number] + xSizeSt / 2) && xpos > (xPosSt[sticker_number] - xSizeSt / 2))
	{
		if ((height - ypos) < (yPosSt + ySizeSt / 2) && ((height - ypos) > yPosSt - ySizeSt / 2))
		{
			return true;
		}
	}
	return false;
}