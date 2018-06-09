-- main.lua

---------------------------------------------------------------------------------------------------
-- #Terms
-- + PixelPosition 
-- 		- The TOP LEFT corner pixel coordinate of an actor
--		- The 'position' value of every actor is PixelPosition
-- + TilePosition
--		- The tile coordinate which contain the CENTER of an actor

-- Variables
--------------------------------------------
-- Pacman
kPacmanSize = {width = 16, height = 16}
kPacmanStartPos = {x = 12, y = 28}

-- Ghost
kGhostStartIndex = 2
kGhostCount = 4
kGhostMoveSpeed = 1
kGhostStartingPosition = {x = 5, y = 50}
kGhostSize = {width = 21, height = 20}

-- Node (Deprecated)
kStartingNodeIndex = 3
kNodeCount = 65
kNodeSize = {width = 24, height = 24}
kNodeSizeInTiled = {width = 8, height = 8}

-- Input
queuedMovementDirection = {x = 0, y = 0}

-- Others
kStay = {x = 0, y = 0}
kLeft = {x = -1, y = 0}
kRight = {x = 1, y = 0}
kUp = { x = 0, y = -1}
kDown = { x = 0, y = 1}

kUpAnimPos =  80
kDownAnimPos = 112
kLeftAnimPos = 48
kRightAnimPos = 16

gFoodCount = 172
gFoodTimer = 0
kSuperPacmanDuration = 20
kMaxHealth = 3

-- Tile-
kTileSize = {width = 8, height = 8}

-- Ai State
kAiState = {Caged = 0, Chase = 1, Scatter = 2, Frightened = 3, RunningHome = 4}

-- Ghost Home position
gHomeTilePosition = {}

----------------------------------------------------
-- Initialize Map Nodes
----------------------------------------------------
function InitMapNodes()
	ReadCollisionMap("Assets/Map/PacmanTileMap.txt")

	gHomeTilePosition.x, gHomeTilePosition.y = GetGhostHomePosition()
end

----------------------------------------------------
-- Initialize Pacman
----------------------------------------------------
function InitPacman()
	pacman = Logic.Actors[1]
	pacman.direction = kStay
	pacman.moveSpeed = 1
	pacman.size = kPacmanSize
	pacman.id = 0
	-- State 0 = normal, state 1 = invulnerable
	pacman.state = 0
	pacman.animTimer = 0
	pacman.maxFrame = 2
	pacman.currentFrame = 0
	pacman.animDirection = 1
	pacman.baseAnimPos = 16
	pacman.health = {}
	pacman.health.actor = {}
	pacman.health.amount = kMaxHealth
	pacman.transform = pacman.TransformComponent
	pacman.position = GetPosition(pacman.transform.this)
	pacman.isAlive = true


	for i=1, kMaxHealth do	
		pacman.health.actor[i] = SpawnActor(Logic.this, "Assets\\XML\\PacmanHealth.xml")	
		local healthTransform = pacman.health.actor[i].TransformComponent
		local healthPosition = GetPosition(healthTransform.this)
		healthTransform.SetPosition(healthTransform.this, healthPosition.x + ((i -1)* 25), healthPosition.y)
		--change the position
	end

	-- Register this unit to C++
	RegisterUnit(pacman.id, 0, GetPosition(pacman.transform.this))
	
	-- Update C++ information
	UpdateCppActor(pacman)
	
	-- Reaction
	AddReaction(pacman.BoxColliderComponent.this, "Food", "SmallFoodReaction")
	AddReaction(pacman.BoxColliderComponent.this, "LargeFood", "LargeFoodReaction")
	AddReaction(pacman.BoxColliderComponent.this, "Ghost", "GhostReaction")
end

----------------------------------------------------
-- Initialize Ghost
----------------------------------------------------
function InitGhost()
	ghost = {}
	
	for i=1, kGhostCount do
		ghost[i] = Logic.Actors[kGhostStartIndex + i - 1]
		ghost[i].direction = kUp
		ghost[i].moveSpeed = 1
		ghost[i].size = kGhostSize
		ghost[i].id = i
		ghost[i].runHome = false
		ghost[i].transform = ghost[i].TransformComponent
		ghost[i].position = GetPosition(ghost[i].transform.this)
		RegisterUnit(ghost[i].id, i, ghost[i].position)
		
		-- Update C++ information
		UpdateCppActor(ghost[i])
	end
end

----------------------------------------------------
-- Initialize Score
----------------------------------------------------
function InitScore()
	score = {}
	score.amount = 0
	score.text = SpawnActor(Logic.this, "Assets\\XML\\ScoreText.xml") 
	score.numbers ={}
	
	for i=1, 4 do
		score.numbers[i] = SpawnActor(Logic.this, " Assets\\XML\\Numbers.xml")
		local numberTransform = score.numbers[i].TransformComponent
		local numberPosition = GetPosition(numberTransform.this)
		numberTransform.SetPosition(numberTransform.this, numberPosition.x - ((i -1)* 25), numberPosition.y)
	end
end

----------------------------------------------------
-- Handle the input given by the player
----------------------------------------------------
function HandleInput()
	local command = GetCommand(InputSystem.this)

	-- Set up the pacman direction depending on the input
	if command == "w" then      -- UP
		queuedMovementDirection = kUp
	elseif command == "s" then  -- DOWN
		queuedMovementDirection = kDown
	elseif command == "a" then  -- LEFT
		queuedMovementDirection = kLeft
	elseif command == "d" then  -- RIGHT
		queuedMovementDirection = kRight
	end
end

----------------------------------------------------
-- Get the center position of an object
----------------------------------------------------
function GetCenter(actor)
	return { x = actor.position.x + actor.size.width / 2,
			 y = actor.position.y + actor.size.height / 2 }
end

----------------------------------------------------
-- Convert pixel coordinate to tile coordinate
----------------------------------------------------
function GetTilePosition(actor)
	local center = GetCenter(actor)
	return { x = math.floor(center.x / kTileSize.width), 
			 y = math.floor(center.y / kTileSize.height)}
end

----------------------------------------------------
-- Update actor information in c++
----------------------------------------------------
function UpdateCppActor(actor, deltatime)
	local tilePosition = GetTilePosition(actor)
	UpdatePosition(actor.id, tilePosition.x, tilePosition.y)
	UpdateDirection(actor.id, actor.direction.x, actor.direction.y)
	Update(actor.id, deltatime)
end

----------------------------------------------------
-- Update Pacman position in the game
----------------------------------------------------
function UpdatePacman(deltatime)
	
	-- If pacman is dead, don't run UpdatePacman
	if  pacman.isAlive == false then
		return
	end

	-- Decrease food timer over time
	if gFoodTimer > 0 then
		gFoodTimer = gFoodTimer - deltatime	
	end

	-- Apply pacman animation
	pacman.animTimer = pacman.animTimer + deltatime

	if pacman.animTimer > 0.1 then
		pacman.currentFrame = pacman.currentFrame + 1

		if pacman.currentFrame > pacman.maxFrame or pacman.currentFrame < 0 then
				pacman.animDirection = pacman.animDirection * -1	
				pacman.currentFrame = 0
		end
		pacman.animTimer = 0
	end

	
	local transform = pacman.TransformComponent
	
	-- Update pacman position information
	pacman.position = GetPosition(transform.this)
	
	--teleport player if it is at edge of screen
	if pacman.position.x < 0 then
		transform.SetPosition(transform.this, 300, pacman.position.y)
		pacman.position.x = 300
	end

	if pacman.position.x > 300 then
		transform.SetPosition(transform.this, 28, pacman.position.y)
		pacman.position.x = 28
	end


	-- Update pacman in C++
	UpdateCppActor(pacman, deltatime)
	
	
	
	-- If there is a movement input direction in queue

		pacmanTilePosition = GetTilePosition(pacman)
		local targetTileX = pacmanTilePosition.x + queuedMovementDirection.x
		local targetTileY = pacmanTilePosition.y + queuedMovementDirection.y

		-- Check if the tile has a walkable path to the assigned direction
		if IsWalkable(targetTileX, targetTileY) and queuedMovementDirection ~= kStay then
			-- Set the pacman direction towards the walkable tile
			pacman.direction = queuedMovementDirection
			
		elseif not IsWalkable(pacmanTilePosition.x + pacman.direction.x,
							pacmanTilePosition.y + pacman.direction.y) then
			pacman.direction = kStay
		end		
		
	
	-- Calculate the velocity
	local velocityX = pacman.direction.x * pacman.moveSpeed
	local velocityY = pacman.direction.y * pacman.moveSpeed

	local squareRender = pacman.SquareRenderComponent

	--Animate the pacman
	if(velocityX > 0) then
		pacman.baseAnimPos = kRightAnimPos
		squareRender.SetSource(squareRender.this, kRightAnimPos + (kPacmanSize.width * (pacman.currentFrame - 1) * pacman.currentFrame/pacman.currentFrame), 0)
	elseif (velocityX < 0) then
		pacman.baseAnimPos = kLeftAnimPos
		squareRender.SetSource(squareRender.this,kLeftAnimPos + (kPacmanSize.width  * (pacman.currentFrame - 1) * pacman.currentFrame/pacman.currentFrame), 0)
	elseif (velocityY > 0) then
		pacman.baseAnimPos = kDownAnimPos
		squareRender.SetSource(squareRender.this, kDownAnimPos + (kPacmanSize.width  * (pacman.currentFrame - 1) * pacman.currentFrame/pacman.currentFrame), 0)
	elseif (velocityY < 0) then
		pacman.baseAnimPos = kUpAnimPos
		squareRender.SetSource(squareRender.this, kUpAnimPos + (kPacmanSize.width  * (pacman.currentFrame - 1) * pacman.currentFrame/pacman.currentFrame), 0)
	else
		squareRender.SetSource(squareRender.this, pacman.baseAnimPos, 0)
	end
	
	if queuedMovementDirection == kStay then
			pacman.direction = kStay

		return
	end

	-- Move the Pacman
	transform.Move(transform.this, velocityX, velocityY)
end

function UpdateScore(deltatime)
	for i=1, 4 do
		
		local temp = math.floor(score.amount / math.pow(10, (i-1)))
		
		if temp > 9 then
			temp = temp % 10
		end
		score.numbers[i].SquareRenderComponent.SetSource(score.numbers[i].SquareRenderComponent.this, temp* 20, 0);  
	end
end 
----------------------------------------------------
-- Check if the two directions are opposite of each other
----------------------------------------------------
function IsOppositeDirection(direction1, direction2)
	if (direction1.x == direction2.x * (-1)) and (direction1.y == direction2.y * (-1)) then
		return true
	else
		return false
	end
end

----------------------------------------------------
-- Update Ghost position in the game based on AI
----------------------------------------------------
function UpdateGhost(deltatime)
	for i=1, kGhostCount do
		local transform = ghost[i].TransformComponent
		
		--TODO: Find a better way than setting it every frame
		if ghost[i].runHome == true then
			ghost[i].SquareRenderComponent.SetSource(ghost[i].SquareRenderComponent.this, 42, 0);  
		elseif gFoodTimer > 0 then
			ghost[i].SquareRenderComponent.SetSource(ghost[i].SquareRenderComponent.this, 21, 0);  
		elseif gFoodTimer <= 0 then
			ghost[i].SquareRenderComponent.SetSource(ghost[i].SquareRenderComponent.this, 0, 0);  
		end

		-- Update ghost position information
		ghost[i].position = GetPosition(transform.this)
		
		--teleport player if it is at edge of screen
		if ghost[i].position.x < 0 then
			transform.SetPosition(transform.this, 300, ghost[i].position.y)
			ghost[i].position.x = 300
		end

		if ghost[i].position.x > 300 then
			transform.SetPosition(transform.this, 28, ghost[i].position.y)
			ghost[i].position.x = 28
		end

		-- Update ghost in C++
		UpdateCppActor(ghost[i], deltatime)
		
		-- Get ghost ai movement 
		-- TODO: Find a better way to do this
		local newMove = {x = 0, y = 0}

		-- If the ghost is running home, find the path to home
		if (ghost[i].runHome) then
			local ghostPosition = GetTilePosition(ghost[i])
			-- Check if the ghost already arrive at home
			if ghostPosition.x == gHomeTilePosition.x and ghostPosition.y == gHomeTilePosition.y then
				ghost[i].runHome = false
			-- If the ghost hasn't arrive home, find the path to home
			else
				newMove.x, newMove.y = FindPathHome(ghost[i].id)
			end
		end

		-- If the ghost is not running home
		if ghost[i].runHome == false then
			newMove.x, newMove.y = GetAiMove(ghost[i].id)
		end
		
		ghost[i].direction = {x = newMove.x, y = newMove.y}
		
		-- Calculate Velocity
		velocityX = ghost[i].direction.x * ghost[i].moveSpeed
		velocityY = ghost[i].direction.y * ghost[i].moveSpeed
		
		--Move the ghost
		transform.Move(transform.this, velocityX, velocityY)
	end
end

----------------------------------------------------
-- A reaction function that is called when colliding with food
----------------------------------------------------
function SmallFoodReaction(actor, other)
	score.amount = 	score.amount + 10

	DestroyActor(other.this)

end


----------------------------------------------------
-- A reaction function that is called when colliding with food
----------------------------------------------------
function LargeFoodReaction(actor, other)
		
	-- Become Super Pacman
	gFoodTimer = kSuperPacmanDuration
	
	-- Set all ghost to Frightened
	for i=1, kGhostCount do
		SetGhostFrightenedStatus(ghost[i].id, true)
	end
		
	DestroyActor(other.this)
end

----------------------------------------------------
-- A reaction function that is called when colliding with ghost
----------------------------------------------------
function GhostReaction(actor, other)
	if gFoodTimer <= 0 then
		-- Decrease pacman health by 1
		DestroyActor(actor.health.actor[actor.health.amount].this)
		actor.health.amount = actor.health.amount -1 

		-- If pacman health is less than 0, game over
		if actor.health.amount == 0 then
			pacman.isAlive = false
			SpawnActor(Logic.this, "Assets\\XML\\GameOver.xml")	
			DestroyActor(actor.this)
			return
		end

		-- If pacman still alives, reset everyone position
		local transform = actor.TransformComponent;
		transform.SetPosition(transform.this, 12, 15)

		for i=1, kGhostCount do
			ghost[i].TransformComponent.SetPosition(ghost[i].TransformComponent.this, 154 ,190)
		end

		-- Set queued movement to stay
		queuedMovementDirection = kStay
		actor.SquareRenderComponent.SetSource(actor.SquareRenderComponent.this, 16, 0)

	-- If Pacman is in 'Super' mode
	else
		if other.runHome then
			
		end
		-- Add score
		score.amount = 	score.amount + 100
		other.runHome = true
		SetGhostFrightenedStatus(other.id, false);
	end


end
----------------------------------------------------
-- Run once when the game started
----------------------------------------------------
function LuaInit()
	InitMapNodes()
	InitPacman()
	InitGhost()
	InitScore()
end

----------------------------------------------------
-- Run every frame
----------------------------------------------------
function LuaUpdate(deltatime)
	HandleInput()

	-- Special update only if pacman is in super mode
	if gFoodTimer > 0 then
		-- Decrease Food Timer
		gFoodTimer = gFoodTimer - deltatime

		-- If the timer runs out
		if gFoodTimer <= 0 then
			-- Set all ghost state to Chase
			for i=1, kGhostCount do
				SetGhostFrightenedStatus(ghost[i].id, false);
			end
		end
	end
	
	UpdatePacman(deltatime)
	UpdateGhost(deltatime)
	UpdateScore(deltatime)
end