#pragma once

class Texture
{
public:
	~Texture();

	enum Mode
	{
		RepeatX = 0x01,
		RepeatY = 0x02,
		LinearFilterMin = 0x04,
		LinearFilterMag = 0x08,
		LinearFilter = LinearFilterMin | LinearFilterMag,
		Repeat = RepeatX | RepeatY,
		Default = LinearFilter
	};

	void mode(Mode mode);
	int width() const;
	int height() const;

private:
	Texture(Graphics *graphics);
	bool load(const char *path, Mode mode);
	GLuint id() const;

	Graphics *graphics_;
	GLuint textureId_;
	int width_;
	int height_;

	friend class Graphics;
};
