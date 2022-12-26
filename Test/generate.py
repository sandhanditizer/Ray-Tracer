import math
import random
import numpy as np


def shift_triangle(vertices, shift_x, shift_y, shift_z):
    shifted_vertices = []
    for x, y, z in vertices:
        shifted_vertices.append([x + shift_x, y + shift_y, z + shift_z])
        
    return shifted_vertices


# Rotates a given triangle around the x, y, and z axies

def rotate_triangle_x(triangle, angle_degrees):
    angle_radians = math.radians(angle_degrees)

    rotation_matrix = np.array([
        [1, 0, 0],
        [0, np.cos(angle_radians),  -np.sin(angle_radians)],
        [0, np.sin(angle_radians), np.cos(angle_radians)]
    ])

    rotated_triangle = np.dot(triangle, rotation_matrix)

    return rotated_triangle

def rotate_triangle_y(triangle, angle_degrees):
    angle_radians = math.radians(angle_degrees)

    rotation_matrix = np.array([
        [np.cos(angle_radians), 0,  -np.sin(angle_radians)],
        [0, 1, 0],
        [np.sin(angle_radians),  0, np.cos(angle_radians)]
    ])

    rotated_triangle = np.dot(triangle, rotation_matrix)

    return rotated_triangle

def rotate_triangle_z(triangle, angle_degrees):
    angle_radians = math.radians(angle_degrees)

    rotation_matrix = np.array([
        [np.cos(angle_radians), -np.sin(angle_radians), 0],
        [np.sin(angle_radians),  np.cos(angle_radians), 0],
        [0, 0, 1]
    ])

    rotated_triangle = np.dot(triangle, rotation_matrix)

    return rotated_triangle

def generate_triangle(radius):
    vertices = []
    for i in range(3):
        angle = 2 * math.pi * (i/3)
        x = radius * math.cos(angle)
        z = radius * math.sin(angle)
        y = math.sqrt(x**2 + z**2)
        vertices.append([x, y, z])
                
    return vertices


# For making a christmas tree
def random_cone_points(height, base):
    base_radius = base / 2
    points = []

    # Generate random x and y coordinates given base size
    x = random.uniform(-base_radius, base_radius)
    y = random.uniform(-base_radius, base_radius)

    # Calculate the distance of the point from the center of the cone's base
    distance = ((x**2) + (y**2))**0.5

    slope = height / base_radius

    z = (-distance * slope + height) % height
    
    points.append([x, y, 0])
    
    choice = [-1, 1]
    a = random.choice(choice)
    b = random.choice(choice)
    c = random.randint(-2, 2)
    
    if a == 1 and b == 1:
        points.append([-x, -y, 0])
    else:
        points.append([a*x, b*y, 0])
        

    a = random.uniform(-0.5, 0.5)
    b = random.uniform(-0.5, 0.5)
    points.append([a, b, z])

    return points


# -------------------------------------------------------------

file_name = 'snowflakes_mesh.txt'

num_snowflakes = 300
x_rotation = 40
y_rotation = 360
z_rotation = 70
snowflakes = []
for _ in range(0, num_snowflakes):
    radius = random.uniform(0.01, 0.15) # Adjusting the size of the snowflakes
    t = generate_triangle(radius)
    rt = rotate_triangle_y(t, 60)
    
    # Random rotation around the x axis
    a = random.uniform(-x_rotation, x_rotation)
    t = rotate_triangle_x(t, a)
    rt = rotate_triangle_x(rt, a)
    
    # Random rotation around the y axis
    a = random.uniform(0, y_rotation)
    t = rotate_triangle_y(t, a)
    rt = rotate_triangle_y(rt, a)
    
    # Random rotation around the z axis
    a = random.uniform(-z_rotation, z_rotation)
    t = rotate_triangle_z(t, a)
    rt = rotate_triangle_z(rt, a)
    
    x, y, z = random.uniform(-15, 15), random.uniform(-30, 50), random.uniform(-1, 10) # Random shifting 
    st = shift_triangle(t, x, y, z)
    srt = shift_triangle(rt, x, y, z)
    snowflakes.append([st, srt])
    
    
with open(f'Meshes/{file_name}', 'w') as f:
    f.write("vertices\n")
    count = 0
    for snowflake in snowflakes:
        for vertex in snowflake[0]:
            f.write(f"({vertex[0]} {vertex[1]} {vertex[2]})\n")
            count += 1
        
        f.write("\n\n")
        for vertex in snowflake[1]:
            f.write(f"({vertex[0]} {vertex[1]} {vertex[2]})\n")
            count += 1
        f.write("\n\n")

    f.write(f"triangles\n")
    
    for i in range(0, count, 3):
        f.write(f"{i} {i+1} {i+2}\n")
    


file_name = 'leafs_mesh.txt'

num_leafs = 50
leafs = []
for _ in range(0, num_leafs):
    leafs.append(random_cone_points(12, 6))
    
    
with open(f'Meshes/{file_name}', 'w') as f:
    f.write("vertices\n")
    count = 0
    for leaf in leafs:
        for vertex in leaf:
            f.write(f"({vertex[0]} {vertex[1]} {vertex[2]})\n")
            count += 1
            
        f.write("\n\n")

    f.write(f"triangles\n")
    for i in range(0, count, 3):
        f.write(f"{i} {i+1} {i+2}\n")
    
