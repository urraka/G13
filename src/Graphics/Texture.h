#pragma once

class Texture
{
public:
	~Texture();

	enum Mode
	{
		kRepeatX = 0x01,
		kRepeatY = 0x02,
		kLinearFilterMin = 0x04,
		kLinearFilterMag = 0x08,
		kLinearFilter = kLinearFilterMin | kLinearFilterMag,
		kRepeat = kRepeatX | kRepeatY,
		kDefault = kLinearFilter
	};

	void bind();
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
