void init_ray(Ray& ray, const Player& player, double angle);
void calculate_step(Ray& ray, const Player& player);
void ray_step(Ray& ray);
bool is_hit(const Ray& ray);
void calculate_perpetual(Ray& ray, const Game& game);
void draw_line(const Ray& ray, int& x);



void raycast(const Game& game)
{

	double angle_step = FOV / WIDTH; // ray count
	double angle = game.player.dir - FOV / 2;

	for (int x = 0; x < WIDTH; x++, angle += angle_step)
	{
		Ray ray;

		init_ray(ray, game.player, angle);
		//calculate_step(&ray, &(game->player));
		calculate_perpetual(ray, game);
    draw_line(ray, x); //takes 60ms
	}


}


void init_ray(Ray& ray, const Player& player, double angle)
{
	ray.dir.x = cos(angle);
	ray.dir.y = sin(angle);

	ray.delta_dist.x = fabs(1 / ray.dir.x);
	ray.delta_dist.y = fabs(1 / ray.dir.y);

	ray.p_map.x = (int)player.x;
	ray.p_map.y = (int)player.y;

	ray.hit = 0;

	ray.angle = angle;

  calculate_step(ray, player);

  ray.color = WALL;
}

bool is_hit(const Ray& ray)
{
	if (ray.p_map.x < 0 || ray.p_map.y < 0)
		return (1);
	if (ray.p_map.x >= MAP_SIZE || ray.p_map.y >= MAP_SIZE)
		return (1);
	if (game_map[(int)ray.p_map.y][(int)ray.p_map.x] == 1)
		return (1);

	return (0);
}

void ray_step(Ray& ray)
{
	if (ray.side_dist.x < ray.side_dist.y)
	{
		ray.side_dist.x += ray.delta_dist.x;
		ray.p_map.x += ray.step.x;
		ray.side = 0;
	}
	else
	{
		ray.side_dist.y += ray.delta_dist.y;
		ray.p_map.y += ray.step.y;
		ray.side = 1;
	}
}

//only color rn
void determine_texture(Ray& ray, const Game& game)
{

  if (ray.side == 1) 
  {
    ray.color = WALL_2;
  }

}
void calculate_perpetual(Ray& ray, const Game& game)
{
	while (ray.hit == 0)
	{
		ray_step(ray);
		if (is_hit(ray))
			ray.hit = 1;
	}
	if (ray.side == 0) // EAST_WEST
		ray.perp_wall_dist = ray.side_dist.x - ray.delta_dist.x;
	else
		ray.perp_wall_dist = ray.side_dist.y - ray.delta_dist.y;

	determine_texture(ray, game);

  //correct fish eye
	ray.perp_wall_dist = ray.perp_wall_dist * cos(game.player.dir - ray.angle);
}

void calculate_step(Ray& ray, const Player& player)
{
	if (ray.dir.x < 0)
	{
		ray.step.x = -1;
		ray.side_dist.x = (player.x - ray.p_map.x) * ray.delta_dist.x;
	}
	else
	{
		ray.step.x = 1;
		ray.side_dist.x = (ray.p_map.x + 1.0 - player.x) * ray.delta_dist.x;
	}

	if (ray.dir.y < 0)
	{
		ray.step.y = -1;
		ray.side_dist.y = (player.y - ray.p_map.y) * ray.delta_dist.y;
	}
	else
	{
		ray.step.y = 1;
		ray.side_dist.y = (ray.p_map.y + 1.0 - player.y) * ray.delta_dist.y;
	}
}


//const?
void draw_line(const Ray& ray, int& x)
{
	double wall_height = (1 / ray.perp_wall_dist) * HEIGHT;

	int start = (HEIGHT - wall_height) / 2;
	int end = (HEIGHT + wall_height) / 2;

	tft->drawLine(x, 0, x, start, CEILING);
	tft->drawLine(x, start, x, end, ray.color);//ST77XX_RED //wall
	tft->drawLine(x, end, x, HEIGHT, FLOOR);
}
